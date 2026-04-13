#include "podio/ROOTLazyReader.h"
#include "podio/CollectionIDTable.h"
#include "podio/GenericParameters.h"
#include "podio/podioVersion.h"
#include "podio/utilities/RootHelpers.h"

#include "ROOTLazyCategoryState.h"
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

GenericParameters ROOTLazyReader::readEntryParameters(CategoryState& catState, unsigned int localEntry) {
  GenericParameters params;

  if (m_fileVersion < podio::version::Version{0, 99, 99}) {
    // Old format: single parameter branch
    auto& paramBranch = catState.paramBranches[0];
    paramBranch.data = root_utils::getBranch(catState.chain.get(), root_utils::paramBranchName);

    auto* branch = paramBranch.data;
    auto* emd = &params;
    branch->SetAddress(&emd);
    branch->GetEntry(localEntry);
  } else {
    root_utils::readParams<int>(catState.paramBranches, catState.chain.get(), params, true, localEntry);
    root_utils::readParams<float>(catState.paramBranches, catState.chain.get(), params, true, localEntry);
    root_utils::readParams<double>(catState.paramBranches, catState.chain.get(), params, true, localEntry);
    root_utils::readParams<std::string>(catState.paramBranches, catState.chain.get(), params, true, localEntry);
  }

  return params;
}

std::unique_ptr<ROOTLazyFrameData> ROOTLazyReader::readNextEntry(std::string_view name,
                                                                 const std::vector<std::string>& collsToRead) {
  auto& catState = getCategoryState(name);
  if (!catState) {
    return nullptr;
  }
  auto result = readEntry(catState, catState->entry, collsToRead);
  if (result) {
    catState->entry++;
  }
  return result;
}

std::unique_ptr<ROOTLazyFrameData> ROOTLazyReader::readEntry(std::string_view name, const unsigned entry,
                                                             const std::vector<std::string>& collsToRead) {
  auto& catState = getCategoryState(name);
  auto result = readEntry(catState, entry, collsToRead);
  if (result) {
    catState->entry = entry + 1;
  }
  return result;
}

std::unique_ptr<ROOTLazyFrameData> ROOTLazyReader::readEntry(std::shared_ptr<CategoryState>& catState, unsigned entry,
                                                             const std::vector<std::string>& collsToRead) {
  if (!catState || !catState->chain) {
    return nullptr;
  }
  if (entry >= catState->chain->GetEntries()) {
    return nullptr;
  }

  // Validate requested collections
  if (!collsToRead.empty()) {
    for (const auto& name : collsToRead) {
      if (std::ranges::find(catState->storedClasses, name, &NamedCollInfo::name) == catState->storedClasses.end()) {
        throw std::invalid_argument(name + " is not available from Frame");
      }
    }
  }

  podio::GenericParameters parameters;
  {
    std::lock_guard lock{catState->mutex};

    const auto localEntry = catState->chain->LoadTree(entry);

    // Read parameters eagerly
    parameters = readEntryParameters(*catState, localEntry);
  }

  // Build available collections map (no lock needed, just reading metadata)
  std::unordered_map<std::string, size_t> availableCollections;
  for (size_t i = 0; i < catState->storedClasses.size(); ++i) {
    if (!collsToRead.empty() && std::ranges::find(collsToRead, catState->storedClasses[i].name) == collsToRead.end()) {
      continue;
    }
    availableCollections.emplace(catState->storedClasses[i].name, i);
  }

  return std::make_unique<ROOTLazyFrameData>(catState, entry, std::move(availableCollections), catState->table,
                                             std::move(parameters));
}

void ROOTLazyReader::initCategory(CategoryState& catState, std::string_view category) {
  auto* collInfoBranch = root_utils::getBranch(m_metaChain.get(), root_utils::collInfoName(category));

  auto collInfo = std::vector<root_utils::CollectionWriteInfo>();
  auto* collInfoPtr = &collInfo;
  if (m_fileVersion >= podio::version::Version{1, 2, 999}) {
    collInfoBranch->SetAddress(&collInfoPtr);
    collInfoBranch->GetEntry(0);
  } else {
    auto collInfoOld = std::vector<root_utils::CollectionWriteInfoT>();
    if (m_fileVersion < podio::version::Version{0, 16, 4}) {
      auto collInfoReallyOld = std::vector<root_utils::CollectionInfoWithoutSchemaT>();
      auto* tmpPtr = &collInfoReallyOld;
      collInfoBranch->SetAddress(&tmpPtr);
      collInfoBranch->GetEntry(0);
      collInfoOld.reserve(collInfoReallyOld.size());
      for (const auto& [collID, collType, isSubsetColl] : collInfoReallyOld) {
        collInfo.emplace_back(collID, std::move(collType), isSubsetColl, 1u);
      }
    } else {
      auto* tmpPtr = &collInfoOld;
      collInfoBranch->SetAddress(&tmpPtr);
      collInfoBranch->GetEntry(0);
    }
    collInfo.reserve(collInfoOld.size());
    for (const auto& [id, typeName, isSubsetColl, schemaVersion] : collInfoOld) {
      collInfo.emplace_back(id, std::move(typeName), isSubsetColl, schemaVersion);
    }
  }

  // Recreate the idTable from the collection info if necessary
  if (m_fileVersion >= podio::version::Version{1, 2, 999}) {
    catState.table = root_utils::makeCollIdTable(collInfo);
  } else {
    catState.table = std::make_shared<podio::CollectionIDTable>();
    const auto* table = catState.table.get();
    auto* tableBranch = root_utils::getBranch(m_metaChain.get(), root_utils::idTableName(category));
    tableBranch->SetAddress(&table);
    tableBranch->GetEntry(0);
  }

  // Set up collection branches
  if (m_fileVersion < podio::version::Version{0, 16, 99}) {
    std::tie(catState.branches, catState.storedClasses) =
        createCollectionBranchesIndexBased(catState.chain.get(), *catState.table, collInfo);
  } else {
    std::tie(catState.branches, catState.storedClasses) =
        createCollectionBranches(catState.chain.get(), *catState.table, collInfo);
  }

  // Set up parameter branches separately from collection branches
  if (m_fileVersion < podio::version::Version{0, 99, 99}) {
    catState.paramBranches.emplace_back(root_utils::getBranch(catState.chain.get(), root_utils::paramBranchName));
  } else {
    catState.paramBranches.emplace_back(root_utils::getBranch(catState.chain.get(), root_utils::intKeyName));
    catState.paramBranches.emplace_back(root_utils::getBranch(catState.chain.get(), root_utils::intValueName));

    catState.paramBranches.emplace_back(root_utils::getBranch(catState.chain.get(), root_utils::floatKeyName));
    catState.paramBranches.emplace_back(root_utils::getBranch(catState.chain.get(), root_utils::floatValueName));

    catState.paramBranches.emplace_back(root_utils::getBranch(catState.chain.get(), root_utils::doubleKeyName));
    catState.paramBranches.emplace_back(root_utils::getBranch(catState.chain.get(), root_utils::doubleValueName));

    catState.paramBranches.emplace_back(root_utils::getBranch(catState.chain.get(), root_utils::stringKeyName));
    catState.paramBranches.emplace_back(root_utils::getBranch(catState.chain.get(), root_utils::stringValueName));
  }
}

std::shared_ptr<CategoryState>& ROOTLazyReader::getCategoryState(std::string_view category) {
  if (auto it = m_categoryStates.find(category); it != m_categoryStates.end()) {
    // Use branches as proxy to check whether this category has been initialized
    if (it->second->branches.empty()) {
      initCategory(*it->second, category);
    }
    return it->second;
  }

  // Return a static nullptr for unknown categories
  static auto invalidState = std::shared_ptr<CategoryState>(nullptr);
  return invalidState;
}

void ROOTLazyReader::openFile(const std::string& filename) {
  openFiles({filename});
}

void ROOTLazyReader::openFiles(const std::vector<std::string>& filenames) {
  m_metaChain = std::make_unique<TChain>(root_utils::metaTreeName);
  for (const auto& filename : filenames) {
    if (!m_metaChain->Add(filename.c_str(), -1)) {
      throw std::runtime_error("File " + filename + " couldn't be found or the \"" + root_utils::metaTreeName +
                               "\" tree couldn't be read.");
    }
  }

  auto* versionPtr = &m_fileVersion;
  if (auto* versionBranch = root_utils::getBranch(m_metaChain.get(), root_utils::versionBranchName)) {
    versionBranch->SetAddress(&versionPtr);
    versionBranch->GetEntry(0);
  }

  if (auto* edmDefBranch = root_utils::getBranch(m_metaChain.get(), root_utils::edmDefBranchName)) {
    auto datamodelDefs = DatamodelDefinitionHolder::MapType{};
    auto* datamodelDefsPtr = &datamodelDefs;
    edmDefBranch->SetAddress(&datamodelDefsPtr);
    edmDefBranch->GetEntry(0);

    DatamodelDefinitionHolder::VersionList edmVersions{};
    for (const auto& [name, _] : datamodelDefs) {
      if (auto* edmVersionBranch = root_utils::getBranch(m_metaChain.get(), root_utils::edmVersionBranchName(name))) {
        const auto edmVersion = podio::version::Version{};
        auto* tmpPtr = &edmVersion;
        edmVersionBranch->SetAddress(&tmpPtr);
        edmVersionBranch->GetEntry(0);
        edmVersions.emplace_back(name, edmVersion);
      }
    }

    m_datamodelHolder = DatamodelDefinitionHolder(std::move(datamodelDefs), std::move(edmVersions));
  }

  // Set up categories and their chains
  m_availCategories = podio::root_utils::getAvailableCategories(m_metaChain.get());
  for (const auto& cat : m_availCategories) {
    auto state = std::make_shared<CategoryState>();
    state->chain = std::make_unique<TChain>(cat.c_str());
    for (const auto& fn : filenames) {
      state->chain->Add(fn.c_str());
    }
    m_categoryStates.try_emplace(cat, std::move(state));
  }
}

unsigned ROOTLazyReader::getEntries(std::string_view name) const {
  if (const auto it = m_categoryStates.find(name); it != m_categoryStates.end()) {
    return it->second->chain->GetEntries();
  }
  return 0;
}

std::vector<std::string_view> ROOTLazyReader::getAvailableCategories() const {
  std::vector<std::string_view> cats;
  cats.reserve(m_categoryStates.size());
  for (const auto& [cat, _] : m_categoryStates) {
    cats.emplace_back(cat);
  }
  return cats;
}

} // namespace podio
