#include "podio/ROOTFrameReader.h"
#include "podio/CollectionBase.h"
#include "podio/CollectionBuffers.h"
#include "podio/CollectionIDTable.h"
#include "podio/GenericParameters.h"
#include "rootUtils.h"

// ROOT specific includes
#include "TChain.h"
#include "TClass.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeCache.h"

#include <unordered_map>

namespace podio {

std::tuple<std::vector<root_utils::CollectionBranches>,
           std::vector<std::pair<std::string, ROOTFrameReader::CollectionInfo>>>
createCollectionBranches(TChain* chain, const podio::CollectionIDTable& idTable,
                         const std::vector<root_utils::CollectionInfoT>& collInfo);

GenericParameters ROOTFrameReader::readEventMetaData(ROOTFrameReader::CategoryInfo& catInfo) {
  // Parameter branch is always the last one
  auto& paramBranches = catInfo.branches.back();
  auto* branch = paramBranches.data;

  GenericParameters params;
  auto* emd = &params;
  branch->SetAddress(&emd);
  branch->GetEntry(catInfo.entry);
  return params;
}

std::unique_ptr<ROOTRawData> ROOTFrameReader::readNextFrame(const std::string& category) {
  auto& catInfo = getCategoryInfo(category);
  if (!catInfo.chain) {
    return nullptr;
  }
  if (catInfo.entry >= catInfo.chain->GetEntries()) {
    return nullptr;
  }

  ROOTRawData::BufferMap buffers;
  for (size_t i = 0; i < catInfo.storedClasses.size(); ++i) {
    buffers.emplace(catInfo.storedClasses[i].first, getCollectionBuffers(catInfo, i));
  }

  auto parameters = readEventMetaData(catInfo);

  catInfo.entry++;
  return std::make_unique<ROOTRawData>(std::move(buffers), catInfo.table, std::move(parameters));
}

podio::CollectionReadBuffers ROOTFrameReader::getCollectionBuffers(ROOTFrameReader::CategoryInfo& catInfo,
                                                                   size_t iColl) {
  const auto& name = catInfo.storedClasses[iColl].first;
  const auto& [theClass, collectionClass, index] = catInfo.storedClasses[iColl].second;
  auto& branches = catInfo.branches[index];

  // Create empty collection buffers, and connect them to the right branches
  auto collBuffers = podio::CollectionReadBuffers();
  // If we have a valid data buffer class we know that have to read data,
  // otherwise we are handling a subset collection
  const bool isSubsetColl = theClass == nullptr;
  if (!isSubsetColl) {
    collBuffers.data = theClass->New();
  }

  {
    auto collection =
        std::unique_ptr<podio::CollectionBase>(static_cast<podio::CollectionBase*>(collectionClass->New()));
    collection->setSubsetCollection(isSubsetColl);

    auto tmpBuffers = collection->createBuffers();
    collBuffers.createCollection = std::move(tmpBuffers.createCollection);
    collBuffers.recast = std::move(tmpBuffers.recast);

    if (auto* refs = tmpBuffers.references) {
      collBuffers.references = new podio::CollRefCollection(refs->size());
    }
    if (auto* vminfo = tmpBuffers.vectorMembers) {
      collBuffers.vectorMembers = new podio::VectorMembersInfo();
      collBuffers.vectorMembers->reserve(vminfo->size());

      for (const auto& [type, _] : (*vminfo)) {
        const auto* vecClass = TClass::GetClass(("vector<" + type + ">").c_str());
        collBuffers.vectorMembers->emplace_back(type, vecClass->New());
      }
    }
  }

  const auto localEntry = catInfo.chain->LoadTree(catInfo.entry);
  // After switching trees in the chain, branch pointers get invalidated so
  // they need to be reassigned.
  // NOTE: root 6.22/06 requires that we get completely new branches here,
  // with 6.20/04 we could just re-set them
  if (localEntry == 0) {
    branches.data = root_utils::getBranch(catInfo.chain.get(), name.c_str());

    // reference collections
    if (auto* refCollections = collBuffers.references) {
      for (size_t i = 0; i < refCollections->size(); ++i) {
        const auto brName = root_utils::refBranch(name, i);
        branches.refs[i] = root_utils::getBranch(catInfo.chain.get(), brName.c_str());
      }
    }

    // vector members
    if (auto* vecMembers = collBuffers.vectorMembers) {
      for (size_t i = 0; i < vecMembers->size(); ++i) {
        const auto brName = root_utils::vecBranch(name, i);
        branches.vecs[i] = root_utils::getBranch(catInfo.chain.get(), brName.c_str());
      }
    }
  }

  // set the addresses and read the data
  root_utils::setCollectionAddresses(collBuffers, branches);
  root_utils::readBranchesData(branches, localEntry);

  collBuffers.recast(collBuffers);

  return collBuffers;
}

ROOTFrameReader::CategoryInfo& ROOTFrameReader::getCategoryInfo(const std::string& category) {
  if (auto it = m_categories.find(category); it != m_categories.end()) {
    // Use the id table as proxy to check whether this category has been
    // initialized alrready
    if (it->second.table == nullptr) {
      initCategory(it->second, category);
    }
    return it->second;
  }

  // Use a nullptr TChain to signify an invalid category request
  // TODO: Warn / log
  static auto invalidCategory = CategoryInfo{nullptr};

  return invalidCategory;
}

void ROOTFrameReader::initCategory(CategoryInfo& catInfo, const std::string& category) {
  catInfo.table = std::make_shared<podio::CollectionIDTable>();
  auto* table = catInfo.table.get();
  auto* tableBranch = root_utils::getBranch(m_metaChain.get(), root_utils::idTableName(category));
  tableBranch->SetAddress(&table);
  tableBranch->GetEntry(0);

  auto* collInfoBranch = root_utils::getBranch(m_metaChain.get(), root_utils::collInfoName(category));
  auto collInfo = new std::vector<root_utils::CollectionInfoT>();
  collInfoBranch->SetAddress(&collInfo);
  collInfoBranch->GetEntry(0);

  std::tie(catInfo.branches, catInfo.storedClasses) =
      createCollectionBranches(catInfo.chain.get(), *catInfo.table, *collInfo);

  delete collInfo;

  // Finaly set up the branches for the paramters
  root_utils::CollectionBranches paramBranches{};
  paramBranches.data = root_utils::getBranch(catInfo.chain.get(), root_utils::paramBranchName);
  catInfo.branches.push_back(paramBranches);
}

std::vector<std::string> getAvailableCategories(TChain* metaChain) {
  auto* branches = metaChain->GetListOfBranches();
  std::vector<std::string> brNames;
  brNames.reserve(branches->GetEntries());
  for (int i = 0; i < branches->GetEntries(); ++i) {
    const std::string name = branches->At(i)->GetName();
    const auto fUnder = name.find("___");
    if (fUnder != std::string::npos) {
      brNames.emplace_back(name.substr(0, fUnder));
    }
  }

  std::sort(brNames.begin(), brNames.end());
  brNames.erase(std::unique(brNames.begin(), brNames.end()), brNames.end());

  return brNames;
}

void ROOTFrameReader::openFile(const std::string& filename) {
  openFiles({filename});
}

void ROOTFrameReader::openFiles(const std::vector<std::string>& filenames) {
  m_metaChain = std::make_unique<TChain>(root_utils::metaTreeName);
  // NOTE: We simply assume that the meta data doesn't change throughout the
  // chain! This essentially boils down to the assumption that all files that
  // are read this way were written with the same settings.
  m_metaChain->Add(filenames[0].c_str());

  podio::version::Version* versionPtr{nullptr};
  if (auto* versionBranch = root_utils::getBranch(m_metaChain.get(), root_utils::versionBranchName)) {
    versionBranch->SetAddress(&versionPtr);
    versionBranch->GetEntry(0);
  }
  m_fileVersion = versionPtr ? *versionPtr : podio::version::Version{0, 0, 0};
  delete versionPtr;

  // Do some work up front for setting up categories and setup all the chains
  // and record the available categories. The rest of the setup follows on
  // demand when the category is first read
  m_availCategories = getAvailableCategories(m_metaChain.get());
  for (const auto& cat : m_availCategories) {
    auto [it, _] = m_categories.try_emplace(cat, std::make_unique<TChain>(cat.c_str()));
    for (const auto& fn : filenames) {
      it->second.chain->Add(fn.c_str());
    }
  }
}

unsigned ROOTFrameReader::getEntries(const std::string& category) const {
  if (auto it = m_categories.find(category); it != m_categories.end()) {
    return it->second.chain->GetEntries();
  }

  return 0;
}

std::tuple<std::vector<root_utils::CollectionBranches>,
           std::vector<std::pair<std::string, ROOTFrameReader::CollectionInfo>>>
createCollectionBranches(TChain* chain, const podio::CollectionIDTable& idTable,
                         const std::vector<root_utils::CollectionInfoT>& collInfo) {

  size_t collectionIndex{0};
  std::vector<root_utils::CollectionBranches> collBranches;
  collBranches.reserve(collInfo.size() + 1);
  std::vector<std::pair<std::string, ROOTFrameReader::CollectionInfo>> storedClasses;
  storedClasses.reserve(collInfo.size());

  for (const auto& [collID, collType, isSubsetColl] : collInfo) {
    // We only write collections that are in the collectionIDTable, so no need
    // to check here
    const auto name = idTable.name(collID);

    root_utils::CollectionBranches branches{};
    const auto collectionClass = TClass::GetClass(collType.c_str());

    // Need the collection here to setup all the branches. Have to manage the
    // temporary collection ourselves
    auto collection =
        std::unique_ptr<podio::CollectionBase>(static_cast<podio::CollectionBase*>(collectionClass->New()));
    collection->setSubsetCollection(isSubsetColl);

    if (!isSubsetColl) {
      // This branch is guaranteed to exist since only collections that are
      // also written to file are in the info metadata that we work with here
      branches.data = root_utils::getBranch(chain, name.c_str());
    }

    const auto buffers = collection->getBuffers();
    for (size_t i = 0; i < buffers.references->size(); ++i) {
      const auto brName = root_utils::refBranch(name, i);
      branches.refs.push_back(root_utils::getBranch(chain, brName.c_str()));
    }

    for (size_t i = 0; i < buffers.vectorMembers->size(); ++i) {
      const auto brName = root_utils::vecBranch(name, i);
      branches.vecs.push_back(root_utils::getBranch(chain, brName.c_str()));
    }

    const std::string bufferClassName = "std::vector<" + collection->getDataTypeName() + ">";
    const auto bufferClass = isSubsetColl ? nullptr : TClass::GetClass(bufferClassName.c_str());

    storedClasses.emplace_back(name, std::make_tuple(bufferClass, collectionClass, collectionIndex++));
    collBranches.push_back(branches);
  }

  return {collBranches, storedClasses};
}

} // namespace podio
