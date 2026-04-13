#include "podio/ROOTReader.h"
#include "podio/CollectionBufferFactory.h"
#include "podio/CollectionBuffers.h"
#include "podio/utilities/RootHelpers.h"
#include "rootUtils.h"

// ROOT specific includes
#include "TChain.h"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace podio {

std::unique_ptr<ROOTFrameData> ROOTReader::readNextEntry(std::string_view name,
                                                         const std::vector<std::string>& collsToRead) {
  auto& catInfo = getCategoryInfo(name);
  return readEntry(catInfo, collsToRead);
}

std::unique_ptr<ROOTFrameData> ROOTReader::readEntry(std::string_view name, const unsigned entNum,
                                                     const std::vector<std::string>& collsToRead) {
  auto& catInfo = getCategoryInfo(name);
  catInfo.entry = entNum;
  return readEntry(catInfo, collsToRead);
}

std::unique_ptr<ROOTFrameData> ROOTReader::readEntry(ROOTReader::CategoryInfo& catInfo,
                                                     const std::vector<std::string>& collsToRead) {
  if (!catInfo.chain) {
    return nullptr;
  }
  if (catInfo.entry >= catInfo.chain->GetEntries()) {
    return nullptr;
  }

  // Make sure to not silently ignore non-existant but requested collections
  if (!collsToRead.empty()) {
    for (const auto& name : collsToRead) {
      if (std::ranges::find(catInfo.storedClasses, name, &NamedCollInfo::name) == catInfo.storedClasses.end()) {
        throw std::invalid_argument(name + " is not available from Frame");
      }
    }
  }

  // After switching trees in the chain, branch pointers get invalidated so
  // they need to be reassigned.
  // NOTE: root 6.22/06 requires that we get completely new branches here,
  // with 6.20/04 we could just re-set them
  const auto preTreeNo = catInfo.chain->GetTreeNumber();
  const auto localEntry = catInfo.chain->LoadTree(catInfo.entry);
  const auto treeChange = catInfo.chain->GetTreeNumber() != preTreeNo;
  // Also need to make sure to handle the first event
  const auto reloadBranches = treeChange || localEntry == 0;

  ROOTFrameData::BufferMap buffers;
  for (size_t i = 0; i < catInfo.storedClasses.size(); ++i) {
    if (!collsToRead.empty() && std::ranges::find(collsToRead, catInfo.storedClasses[i].name) == collsToRead.end()) {
      continue;
    }
    auto collBuffers = getCollectionBuffers(catInfo, i, reloadBranches, localEntry);
    if (!collBuffers) {
      std::cerr << "WARNING: Buffers couldn't be created for collection " << catInfo.storedClasses[i].name
                << " of type " << std::get<std::string>(catInfo.storedClasses[i].info) << " and schema version "
                << std::get<2>(catInfo.storedClasses[i].info) << std::endl;
      continue;
    }
    buffers.emplace(catInfo.storedClasses[i].name, std::move(collBuffers.value()));
  }

  auto parameters =
      readEntryParameters(catInfo.paramBranches, catInfo.chain.get(), m_fileVersion, reloadBranches, localEntry);

  catInfo.entry++;
  return std::make_unique<ROOTFrameData>(std::move(buffers), catInfo.table, std::move(parameters));
}

std::optional<podio::CollectionReadBuffers> ROOTReader::getCollectionBuffers(ROOTReader::CategoryInfo& catInfo,
                                                                             size_t iColl, bool reloadBranches,
                                                                             unsigned int localEntry) {
  const auto& name = catInfo.storedClasses[iColl].name;
  const auto& [collType, isSubsetColl, schemaVersion, index] = catInfo.storedClasses[iColl].info;
  auto& branches = catInfo.branches[index];

  const auto& bufferFactory = podio::CollectionBufferFactory::instance();
  auto maybeBuffers = bufferFactory.createBuffers(collType, schemaVersion, isSubsetColl);

  if (!maybeBuffers) {
    return std::nullopt;
  }

  auto& collBuffers = maybeBuffers.value();

  if (reloadBranches) {
    root_utils::resetBranches(catInfo.chain.get(), branches, name);
  }

  // set the addresses and read the data
  if (!root_utils::setCollectionAddressesReader(collBuffers, branches)) {
    return std::nullopt;
  }
  root_utils::readBranchesData(branches, localEntry);

  return {std::move(collBuffers)};
}

ROOTReader::CategoryInfo& ROOTReader::getCategoryInfo(std::string_view category) {
  if (auto it = m_categories.find(category); it != m_categories.end()) {
    // Use the id table as proxy to check whether this category has been
    // initialized already
    if (it->second.branches.empty()) {
      root_utils::initCategory(it->second, m_metaChain.get(), category, m_fileVersion);
    }
    return it->second;
  }

  // Use a nullptr TChain to signify an invalid category request
  // TODO: Warn / log
  static auto invalidCategory = CategoryInfo{nullptr};

  return invalidCategory;
}

void ROOTReader::openFile(const std::string& filename) {
  openFiles({filename});
}

void ROOTReader::openFiles(const std::vector<std::string>& filenames) {
  openMetaChain(filenames, m_fileVersion, m_datamodelHolder);

  // Do some work up front for setting up categories and setup all the chains
  // and record the available categories. The rest of the setup follows on
  // demand when the category is first read
  for (const auto& cat : m_availCategories) {
    const auto [it, _] = m_categories.try_emplace(cat, std::make_unique<TChain>(cat.c_str()));
    for (const auto& fn : filenames) {
      it->second.chain->Add(fn.c_str());
    }
  }
}

unsigned ROOTReader::getEntries(std::string_view name) const {
  if (const auto it = m_categories.find(name); it != m_categories.end()) {
    return it->second.chain->GetEntries();
  }

  return 0;
}

std::vector<std::string_view> ROOTReader::getAvailableCategories() const {
  std::vector<std::string_view> cats;
  cats.reserve(m_categories.size());
  for (const auto& [cat, _] : m_categories) {
    cats.emplace_back(cat);
  }
  return cats;
}

std::optional<std::map<std::string, SizeStats>> ROOTReader::getSizeStats(std::string_view category) {
  std::map<std::string, SizeStats> stats;
  getCategoryInfo(category); // Ensure category is initialized
  const auto catIt = m_categories.find(category);
  if (catIt == m_categories.end()) {
    return std::nullopt;
  }
  const auto& catInfo = catIt->second;
  for (const auto& branches : catInfo.branches) {
    size_t totalZipBytes = 0;
    size_t totalTotBytes = 0;
    for (const auto& br : branches.vecs) {
      totalZipBytes += br->GetZipBytes("*");
      totalTotBytes += br->GetTotBytes("*");
    }
    for (const auto& br : branches.refs) {
      totalZipBytes += br->GetZipBytes("*");
      totalTotBytes += br->GetTotBytes("*");
    }
    if (branches.data) {
      totalZipBytes += branches.data->GetZipBytes("*");
      totalTotBytes += branches.data->GetTotBytes("*");
      stats[branches.data->GetName()] = {totalZipBytes, static_cast<float>(totalTotBytes) / totalZipBytes};
    } else {
      auto names = branches.refNames[0];
      // This is a subset collection
      // Delete the suffix "_objIdx"
      names.erase(names.end() - 7, names.end());
      stats[names] = {totalZipBytes, static_cast<float>(totalTotBytes) / totalZipBytes};
    }
  }
  return stats;
}

} // namespace podio
