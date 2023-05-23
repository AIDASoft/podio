#include "podio/ROOTFrameWriter.h"
#include "podio/CollectionBase.h"
#include "podio/DatamodelRegistry.h"
#include "podio/Frame.h"
#include "podio/GenericParameters.h"
#include "podio/podioVersion.h"

#include "rootUtils.h"

#include "TTree.h"

#include <algorithm>
#include <cctype>

namespace podio {

ROOTFrameWriter::ROOTFrameWriter(const std::string& filename) {
  m_file = std::make_unique<TFile>(filename.c_str(), "recreate");
}

void ROOTFrameWriter::writeFrame(const podio::Frame& frame, const std::string& category) {
  writeFrame(frame, category, frame.getAvailableCollections());
}

/**
 * Sort the input vector of strings alphabetically, case insensitive.
 *
 * NOTE: This creates a copy of the input vector, so don't use this in tight
 * loops!
 */
std::vector<std::string> sortAlphabeticaly(const std::vector<std::string>& input) {
  auto output = input;
  // Obviously there is no tolower(std::string) in c++, so this is slightly more
  // involved and we make use of the fact that lexicographical_compare works on
  // ranges and the fact that we can feed it a dedicated comparison function,
  // where we convert the strings to lower case char-by-char. The alternative is
  // to make string copies inside the first lambda, transform them to lowercase
  // and then use operator< of std::string, which would be effectively
  // hand-writing what is happening below.
  std::sort(output.begin(), output.end(), [](const auto& lhs, const auto& rhs) {
    return std::lexicographical_compare(
        lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
        [](const auto& cl, const auto& cr) { return std::tolower(cl) < std::tolower(cr); });
  });
  return output;
}

void ROOTFrameWriter::writeFrame(const podio::Frame& frame, const std::string& category,
                                 const std::vector<std::string>& collsToWrite) {
  auto& catInfo = getCategoryInfo(category);
  // Use the TTree as proxy here to decide whether this category has already
  // been initialized
  if (catInfo.tree == nullptr) {
    catInfo.idTable = frame.getCollectionIDTableForWrite();
    catInfo.collsToWrite = sortAlphabeticaly(collsToWrite);
    catInfo.tree = new TTree(category.c_str(), (category + " data tree").c_str());
    catInfo.tree->SetDirectory(m_file.get());
  }

  std::vector<StoreCollection> collections;
  collections.reserve(catInfo.collsToWrite.size());
  for (const auto& name : catInfo.collsToWrite) {
    auto* coll = frame.getCollectionForWrite(name);
    collections.emplace_back(name, const_cast<podio::CollectionBase*>(coll));

    m_datamodelCollector.registerDatamodelDefinition(coll, name);
  }

  // We will at least have a parameters branch, even if there are no
  // collections
  if (catInfo.branches.empty()) {
    initBranches(catInfo, collections, const_cast<podio::GenericParameters&>(frame.getParameters()));

  } else {
    resetBranches(catInfo.branches, collections, &const_cast<podio::GenericParameters&>(frame.getParameters()));
  }

  catInfo.tree->Fill();
}

ROOTFrameWriter::CategoryInfo& ROOTFrameWriter::getCategoryInfo(const std::string& category) {
  if (auto it = m_categories.find(category); it != m_categories.end()) {
    return it->second;
  }

  auto [it, _] = m_categories.try_emplace(category, CategoryInfo{});
  return it->second;
}

void ROOTFrameWriter::initBranches(CategoryInfo& catInfo, const std::vector<StoreCollection>& collections,
                                   /*const*/ podio::GenericParameters& parameters) {
  catInfo.branches.reserve(collections.size() + 1); // collections + parameters

  // First collections
  for (auto& [name, coll] : collections) {
    root_utils::CollectionBranches branches;
    const auto buffers = coll->getBuffers();
    // For subset collections we only fill one references branch
    if (coll->isSubsetCollection()) {
      auto& refColl = (*buffers.references)[0];
      const auto brName = root_utils::subsetBranch(name);
      branches.refs.push_back(catInfo.tree->Branch(brName.c_str(), refColl.get()));
    } else {
      // For "proper" collections we populate all branches, starting with the data
      auto bufferDataType = "vector<" + coll->getDataTypeName() + ">";
      branches.data = catInfo.tree->Branch(name.c_str(), bufferDataType.c_str(), buffers.data);

      const auto relVecNames = podio::DatamodelRegistry::instance().getRelationNames(coll->getValueTypeName());
      if (auto refColls = buffers.references) {
        int i = 0;
        for (auto& c : (*refColls)) {
          const auto brName = root_utils::refBranch(name, relVecNames.relations[i++]);
          branches.refs.push_back(catInfo.tree->Branch(brName.c_str(), c.get()));
        }
      }

      if (auto vmInfo = buffers.vectorMembers) {
        int i = 0;
        for (auto& [type, vec] : (*vmInfo)) {
          const auto typeName = "vector<" + type + ">";
          const auto brName = root_utils::vecBranch(name, relVecNames.vectorMembers[i++]);
          branches.vecs.push_back(catInfo.tree->Branch(brName.c_str(), typeName.c_str(), vec));
        }
      }
    }

    catInfo.branches.push_back(branches);
    catInfo.collInfo.emplace_back(catInfo.idTable.collectionID(name), coll->getTypeName(), coll->isSubsetCollection(),
                                  coll->getSchemaVersion());
  }

  // Also make branches for the parameters
  root_utils::CollectionBranches branches;
  branches.data = catInfo.tree->Branch(root_utils::paramBranchName, &parameters);
  catInfo.branches.push_back(branches);
}

void ROOTFrameWriter::resetBranches(std::vector<root_utils::CollectionBranches>& branches,
                                    const std::vector<ROOTFrameWriter::StoreCollection>& collections,
                                    /*const*/ podio::GenericParameters* parameters) {
  size_t iColl = 0;
  for (auto& coll : collections) {
    const auto& collBranches = branches[iColl];
    root_utils::setCollectionAddresses(coll.second->getBuffers(), collBranches);
    iColl++;
  }

  branches.back().data->SetAddress(&parameters);
}

void ROOTFrameWriter::finish() {
  auto* metaTree = new TTree(root_utils::metaTreeName, "metadata tree for podio I/O functionality");
  metaTree->SetDirectory(m_file.get());

  // Store the collection id table and collection info for reading in the meta tree
  for (/*const*/ auto& [category, info] : m_categories) {
    metaTree->Branch(root_utils::idTableName(category).c_str(), &info.idTable);
    metaTree->Branch(root_utils::collInfoName(category).c_str(), &info.collInfo);
  }

  // Store the current podio build version into the meta data tree
  auto podioVersion = podio::version::build_version;
  metaTree->Branch(root_utils::versionBranchName, &podioVersion);

  auto edmDefinitions = m_datamodelCollector.getDatamodelDefinitionsToWrite();
  metaTree->Branch(root_utils::edmDefBranchName, &edmDefinitions);

  metaTree->Fill();

  m_file->Write();
  m_file->Close();
}

} // namespace podio
