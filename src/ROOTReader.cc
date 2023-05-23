#include "rootUtils.h"

// podio specific includes
#include "podio/CollectionBase.h"
#include "podio/CollectionIDTable.h"
#include "podio/GenericParameters.h"
#include "podio/ROOTReader.h"

// ROOT specific includes
#include "TChain.h"
#include "TClass.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeCache.h"
#include <memory>
#include <stdexcept>

namespace podio {
// todo: see https://github.com/AIDASoft/podio/issues/290
ROOTReader::~ROOTReader() { // NOLINT(modernize-use-equals-default)
}

std::pair<TTree*, unsigned> ROOTReader::getLocalTreeAndEntry(const std::string& treename) {
  auto localEntry = m_chain->LoadTree(m_eventNumber);
  auto* tree = static_cast<TTree*>(m_chain->GetFile()->Get(treename.c_str()));
  return {tree, localEntry};
}

GenericParameters* ROOTReader::readEventMetaData() {
  auto* emd = new GenericParameters();
  auto [evt_metadatatree, entry] = getLocalTreeAndEntry("evt_metadata");
  auto* branch = root_utils::getBranch(evt_metadatatree, "evtMD");
  branch->SetAddress(&emd);
  evt_metadatatree->GetEntry(entry);
  return emd;
}
std::map<int, GenericParameters>* ROOTReader::readCollectionMetaData() {
  auto* emd = new std::map<int, GenericParameters>;
  auto* col_metadatatree = getLocalTreeAndEntry("col_metadata").first;
  auto* branch = root_utils::getBranch(col_metadatatree, "colMD");
  branch->SetAddress(&emd);
  col_metadatatree->GetEntry(0);
  return emd;
}
std::map<int, GenericParameters>* ROOTReader::readRunMetaData() {
  auto* emd = new std::map<int, GenericParameters>;
  auto* run_metadatatree = getLocalTreeAndEntry("run_metadata").first;
  auto* branch = root_utils::getBranch(run_metadatatree, "runMD");
  branch->SetAddress(&emd);
  run_metadatatree->GetEntry(0);
  return emd;
}

CollectionBase* ROOTReader::readCollection(const std::string& name) {
  // has the collection already been constructed?
  auto p =
      std::find_if(begin(m_inputs), end(m_inputs), [&name](const ROOTReader::Input& t) { return t.second == name; });
  if (p != end(m_inputs)) {
    return p->first;
  }

  // Do we know about this collection? If so, read it
  if (const auto& info = m_storedClasses.find(name); info != m_storedClasses.end()) {
    return getCollection(*info);
  }

  // At this point this collection is definitely not in this file, because we
  // have no information on how to construct it in the first place
  return nullptr;
}

CollectionBase* ROOTReader::getCollection(const std::pair<std::string, CollectionInfo>& collInfo) {
  const auto& name = collInfo.first;
  const auto& [theClass, collectionClass, index] = collInfo.second;
  auto& branches = m_collectionBranches[index];

  auto* collection = static_cast<CollectionBase*>(collectionClass->New());
  auto collBuffers = collection->getBuffers();
  // If we have a valid data buffer class we know that have to read data,
  // otherwise we are handling a subset collection
  if (theClass) {
    collBuffers.data = theClass->New();
  } else {
    collection->setSubsetCollection();
  }

  const auto localEntry = m_chain->LoadTree(m_eventNumber);
  // After switching trees in the chain, branch pointers get invalidated so
  // they need to be reassigned.
  // NOTE: root 6.22/06 requires that we get completely new branches here,
  // with 6.20/04 we could just re-set them
  if (localEntry == 0) {
    branches.data = root_utils::getBranch(m_chain, name.c_str());

    // reference collections
    if (auto* refCollections = collBuffers.references) {
      for (size_t i = 0; i < refCollections->size(); ++i) {
        const auto brName = root_utils::refBranch(name, i);
        branches.refs[i] = root_utils::getBranch(m_chain, brName.c_str());
      }
    }

    // vector members
    if (auto* vecMembers = collBuffers.vectorMembers) {
      for (size_t i = 0; i < vecMembers->size(); ++i) {
        const auto brName = root_utils::vecBranch(name, i);
        branches.vecs[i] = root_utils::getBranch(m_chain, brName.c_str());
      }
    }
  }

  // set the addresses
  root_utils::setCollectionAddresses(collection->getBuffers(), branches);

  return readCollectionData(branches, collection, localEntry, name);
}

CollectionBase* ROOTReader::readCollectionData(const root_utils::CollectionBranches& branches,
                                               CollectionBase* collection, Long64_t entry, const std::string& name) {
  // Read all data
  if (branches.data) {
    branches.data->GetEntry(entry);
  }
  for (auto* br : branches.refs) {
    br->GetEntry(entry);
  }
  for (auto* br : branches.vecs) {
    br->GetEntry(entry);
  }

  // do the unpacking
  const auto id = m_table->collectionID(name);
  collection->setID(id);
  collection->prepareAfterRead();

  m_inputs.emplace_back(std::make_pair(collection, name));
  return collection;
}

void ROOTReader::openFile(const std::string& filename) {
  openFiles({filename});
}

void ROOTReader::openFiles(const std::vector<std::string>& filenames) {
  m_chain = new TChain("events");
  for (const auto& filename : filenames) {
    //-1 forces the headers to be read so that
    // the validity of the files can be checked
    if (!m_chain->Add(filename.c_str(), -1)) {
      delete m_chain;
      throw std::runtime_error("File " + filename + " couldn't be found");
    }
  }

  // read the meta data and build the collectionBranches cache
  // NOTE: This is a small pessimization, if we do not read all collections
  // afterwards, but it makes the handling much easier in general
  auto metadatatree = static_cast<TTree*>(m_chain->GetFile()->Get("metadata"));
  m_table = std::make_shared<podio::CollectionIDTable>();
  auto* table = m_table.get();
  metadatatree->SetBranchAddress("CollectionIDs", &table);

  podio::version::Version* versionPtr{nullptr};
  if (auto* versionBranch = root_utils::getBranch(metadatatree, "PodioVersion")) {
    versionBranch->SetAddress(&versionPtr);
    metadatatree->GetEntry(0);
  }
  m_fileVersion = versionPtr ? *versionPtr : podio::version::Version{0, 0, 0};

  // Read the collection type info
  // For versions <0.13.1 it does not exist and has to be rebuilt from scratch
  if (m_fileVersion < podio::version::Version{0, 13, 1}) {

    std::cout << "PODIO: Reconstructing CollectionTypeInfo branch from other sources in file: \'"
              << m_chain->GetFile()->GetName() << "\'" << std::endl;
    metadatatree->GetEntry(0);
    const auto collectionInfo = root_utils::reconstructCollectionInfo(m_chain, *m_table);
    createCollectionBranches(collectionInfo);

  } else if (m_fileVersion < podio::version::Version{0, 16, 4}) {

    auto* collInfoBranch = root_utils::getBranch(metadatatree, "CollectionTypeInfo");
    auto collectionInfoWithoutSchema = new std::vector<root_utils::CollectionInfoTWithoutSchema>;
    auto collectionInfo = new std::vector<root_utils::CollectionInfoT>;
    collInfoBranch->SetAddress(&collectionInfoWithoutSchema);
    metadatatree->GetEntry(0);
    for (const auto& [collID, collType, isSubsetColl] : *collectionInfoWithoutSchema) {
      collectionInfo->emplace_back(collID, collType, isSubsetColl, 0);
    }
    createCollectionBranches(*collectionInfo);
    delete collectionInfoWithoutSchema;
    delete collectionInfo;

  } else {

    auto* collInfoBranch = root_utils::getBranch(metadatatree, "CollectionTypeInfo");

    auto collectionInfo = new std::vector<root_utils::CollectionInfoT>;
    collInfoBranch->SetAddress(&collectionInfo);
    metadatatree->GetEntry(0);
    createCollectionBranches(*collectionInfo);
    delete collectionInfo;
  }

  delete versionPtr;
}

void ROOTReader::closeFile() {
  closeFiles();
}

void ROOTReader::closeFiles() {
  delete m_chain;
}

void ROOTReader::readEvent() {
  m_chain->GetEntry(m_eventNumber);
  // first prepare all collections in memory...
  for (auto inputs : m_inputs) {
    inputs.first->prepareAfterRead();
  }
  // ...then clean-up the references between them
  //    for(auto inputs : m_inputs){
  //    inputs.first->setReferences(m_registry);

  //  }
}
bool ROOTReader::isValid() const {
  return m_chain->GetFile()->IsOpen() && !m_chain->GetFile()->IsZombie();
}

void ROOTReader::endOfEvent() {
  ++m_eventNumber;
  m_inputs.clear();
}

unsigned ROOTReader::getEntries() const {
  return m_chain->GetEntries();
}

void ROOTReader::goToEvent(unsigned eventNumber) {
  m_eventNumber = eventNumber;
  m_inputs.clear();
}

void ROOTReader::createCollectionBranches(const std::vector<root_utils::CollectionInfoT>& collInfo) {
  size_t collectionIndex{0};

  for (const auto& [collID, collType, isSubsetColl, collSchemaVersion] : collInfo) {
    // We only write collections that are in the collectionIDTable, so no need
    // to check here
    const auto name = m_table->name(collID);

    root_utils::CollectionBranches branches{};
    const auto collectionClass = TClass::GetClass(collType.c_str());

    // Make sure that ROOT actually knows about this datatype before running
    // into a potentially cryptic segmentation fault by accessing the nullptr
    if (!collectionClass) {
      std::cerr << "PODIO: Cannot create the collection type \'" << collType << "\' stored in branch \'" << name
                << "\'. Contents of this branch cannot be read." << std::endl;
      continue;
    }
    // Need the collection here to setup all the branches. Have to manage the
    // temporary collection ourselves
    auto collection =
        std::unique_ptr<podio::CollectionBase>(static_cast<podio::CollectionBase*>(collectionClass->New()));
    collection->setSubsetCollection(isSubsetColl);

    if (!isSubsetColl) {
      // This branch is guaranteed to exist since only collections that are
      // also written to file are in the info metadata that we work with here
      branches.data = root_utils::getBranch(m_chain, name.c_str());
    }

    const auto buffers = collection->getBuffers();
    for (size_t i = 0; i < buffers.references->size(); ++i) {
      const auto brName = root_utils::refBranch(name, i);
      branches.refs.push_back(root_utils::getBranch(m_chain, brName.c_str()));
    }

    for (size_t i = 0; i < buffers.vectorMembers->size(); ++i) {
      const auto brName = root_utils::vecBranch(name, i);
      branches.vecs.push_back(root_utils::getBranch(m_chain, brName.c_str()));
    }

    const std::string bufferClassName = "std::vector<" + collection->getDataTypeName() + ">";
    const auto bufferClass = isSubsetColl ? nullptr : TClass::GetClass(bufferClassName.c_str());

    m_storedClasses.emplace(name, std::make_tuple(bufferClass, collectionClass, collectionIndex++));
    m_collectionBranches.push_back(branches);
  }
}

} // namespace podio
