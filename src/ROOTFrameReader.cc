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

std::pair<TTree*, unsigned> ROOTFrameReader::getLocalTreeAndEntry(const std::string& treename) {
  auto localEntry = m_chain->LoadTree(m_eventNumber);
  auto* tree = static_cast<TTree*>(m_chain->GetFile()->Get(treename.c_str()));
  return {tree, localEntry};
}

GenericParameters ROOTFrameReader::readEventMetaData() {
  GenericParameters params;
  auto [evt_metadatatree, entry] = getLocalTreeAndEntry("evt_metadata");
  auto* branch = root_utils::getBranch(evt_metadatatree, "evtMD");
  // ROOT really needs the address of a pointer for I/O to work properly
  auto* emd = &params;
  branch->SetAddress(&emd);
  evt_metadatatree->GetEntry(entry);
  return params;
}

std::unique_ptr<ROOTRawData> ROOTFrameReader::readNextEvent() {
  std::unordered_map<std::string, podio::CollectionBuffers> buffers;
  for (const auto& collInfo : m_storedClasses) {
    buffers.emplace(collInfo.first, getCollectionBuffers(collInfo));
  }

  auto parameters = readEventMetaData();

  m_eventNumber++;
  return std::make_unique<ROOTRawData>(std::move(buffers), m_table, std::move(parameters));
}

podio::CollectionBuffers ROOTFrameReader::getCollectionBuffers(const std::pair<std::string, CollectionInfo>& collInfo) {
  const auto& name = collInfo.first;
  const auto& [theClass, collectionClass, index] = collInfo.second;
  auto& branches = m_collectionBranches[index];

  // Create empty collection buffers, and connect them to the right branches
  auto collBuffers = podio::CollectionBuffers();
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

  const auto localEntry = m_chain->LoadTree(m_eventNumber);
  // After switching trees in the chain, branch pointers get invalidated so
  // they need to be reassigned.
  // NOTE: root 6.22/06 requires that we get completely new branches here,
  // with 6.20/04 we could just re-set them
  if (localEntry == 0) {
    branches.data = root_utils::getBranch(m_chain.get(), name.c_str());

    // reference collections
    if (auto* refCollections = collBuffers.references) {
      for (size_t i = 0; i < refCollections->size(); ++i) {
        const auto brName = root_utils::refBranch(name, i);
        branches.refs[i] = root_utils::getBranch(m_chain.get(), brName.c_str());
      }
    }

    // vector members
    if (auto* vecMembers = collBuffers.vectorMembers) {
      for (size_t i = 0; i < vecMembers->size(); ++i) {
        const auto brName = root_utils::vecBranch(name, i);
        branches.vecs[i] = root_utils::getBranch(m_chain.get(), brName.c_str());
      }
    }
  }

  // set the addresses and read the data
  root_utils::setCollectionAddresses(collBuffers, branches);
  root_utils::readBranchesData(branches, localEntry);

  return collBuffers;
}

void ROOTFrameReader::openFile(const std::string& filename) {
  openFiles({filename});
}

void ROOTFrameReader::openFiles(const std::vector<std::string>& filenames) {
  m_chain = std::make_unique<TChain>("events");
  for (const auto& filename : filenames) {
    m_chain->Add(filename.c_str());
  }

  // read the meta data and build the collectionBranches cache
  // NOTE: This is a small pessimization, if we do not read all collections
  // afterwards, but it makes the handling much easier in general
  auto metadatatree = static_cast<TTree*>(m_chain->GetFile()->Get("metadata"));
  m_table = std::make_shared<CollectionIDTable>();
  auto* table = m_table.get();
  metadatatree->SetBranchAddress("CollectionIDs", &table);

  podio::version::Version* versionPtr{nullptr};
  if (auto* versionBranch = root_utils::getBranch(metadatatree, "PodioVersion")) {
    versionBranch->SetAddress(&versionPtr);
  }

  // Check if the CollectionTypeInfo branch is there and assume that the file
  // has been written with with podio pre #197 (<0.13.1) if that is not the case
  if (auto* collInfoBranch = root_utils::getBranch(metadatatree, "CollectionTypeInfo")) {
    auto collectionInfo = new std::vector<root_utils::CollectionInfoT>;
    collInfoBranch->SetAddress(&collectionInfo);
    metadatatree->GetEntry(0);
    createCollectionBranches(*collectionInfo);
    delete collectionInfo;
  } else {
    std::cout << "PODIO: Reconstructing CollectionTypeInfo branch from other sources in file: \'"
              << m_chain->GetFile()->GetName() << "\'" << std::endl;
    metadatatree->GetEntry(0);
    const auto collectionInfo = root_utils::reconstructCollectionInfo(m_chain.get(), *m_table);
    createCollectionBranches(collectionInfo);
  }

  m_fileVersion = versionPtr ? *versionPtr : podio::version::Version{0, 0, 0};
  delete versionPtr;
}

unsigned ROOTFrameReader::getEntries() const {
  return m_chain->GetEntries();
}

void ROOTFrameReader::createCollectionBranches(const std::vector<root_utils::CollectionInfoT>& collInfo) {
  size_t collectionIndex{0};

  for (const auto& [collID, collType, isSubsetColl] : collInfo) {
    // We only write collections that are in the collectionIDTable, so no need
    // to check here
    const auto name = m_table->name(collID);

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
      branches.data = root_utils::getBranch(m_chain.get(), name.c_str());
    }

    const auto buffers = collection->getBuffers();
    for (size_t i = 0; i < buffers.references->size(); ++i) {
      const auto brName = root_utils::refBranch(name, i);
      branches.refs.push_back(root_utils::getBranch(m_chain.get(), brName.c_str()));
    }

    for (size_t i = 0; i < buffers.vectorMembers->size(); ++i) {
      const auto brName = root_utils::vecBranch(name, i);
      branches.vecs.push_back(root_utils::getBranch(m_chain.get(), brName.c_str()));
    }

    const std::string bufferClassName = "std::vector<" + collection->getDataTypeName() + ">";
    const auto bufferClass = isSubsetColl ? nullptr : TClass::GetClass(bufferClassName.c_str());

    m_storedClasses.emplace_back(name, std::make_tuple(bufferClass, collectionClass, collectionIndex++));
    m_collectionBranches.push_back(branches);
  }
}

} // namespace podio
