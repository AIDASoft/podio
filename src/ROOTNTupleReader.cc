#include "rootUtils.h"

// podio specific includes
#include "podio/CollectionBase.h"
#include "podio/CollectionIDTable.h"
#include "podio/GenericParameters.h"
#include "podio/ROOTNTupleReader.h"

// ROOT specific includes
#include "TChain.h"
#include "TClass.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeCache.h"
#include <memory>

namespace podio {


GenericParameters* ROOTNTupleReader::readEventMetaData() {
  auto* emd = new GenericParameters();
  // TODO
  //auto [evt_metadatatree, entry] = getLocalTreeAndEntry("evt_metadata");
  //auto* branch = root_utils::getBranch(evt_metadatatree, "evtMD");
  //branch->SetAddress(&emd);
  //evt_metadatatree->GetEntry(entry);
  return emd;
}
std::map<int, GenericParameters>* ROOTNTupleReader::readCollectionMetaData() {
  auto* emd = new std::map<int, GenericParameters>;
  //TODO
  //auto* col_metadatatree = getLocalTreeAndEntry("col_metadata").first;
  //auto* branch = root_utils::getBranch(col_metadatatree, "colMD");
  //branch->SetAddress(&emd);
  //col_metadatatree->GetEntry(0);
  return emd;
}
std::map<int, GenericParameters>* ROOTNTupleReader::readRunMetaData() {
  auto* emd = new std::map<int, GenericParameters>;
  //TODO
  //auto* run_metadatatree = getLocalTreeAndEntry("run_metadata").first;
  //auto* branch = root_utils::getBranch(run_metadatatree, "runMD");
  //branch->SetAddress(&emd);
  //run_metadatatree->GetEntry(0);
  return emd;
}

CollectionBase* ROOTNTupleReader::readCollection(const std::string& name) {
  // has the collection already been constructed?
  //TODO
  //auto p =
  //    std::find_if(begin(m_inputs), end(m_inputs), [&name](const ROOTNTupleReader::Input& t) { return t.second == name; });
  //if (p != end(m_inputs)) {
  //  return p->first;
  //}

  //// Do we know about this collection? If so, read it
  //if (const auto& info = m_storedClasses.find(name); info != m_storedClasses.end()) {
  //  return getCollection(*info);
  //}

  //// At this point this collection is definitely not in this file, because we
  //// have no information on how to construct it in the first place
  //return nullptr;
  return nullptr;
}

CollectionBase* ROOTNTupleReader::getCollection(const std::pair<std::string, CollectionInfo>& collInfo) {
  //const auto& name = collInfo.first;
  //const auto& [theClass, collectionClass, index] = collInfo.second;
  //auto& branches = m_collectionBranches[index];

  //auto* collection = static_cast<CollectionBase*>(collectionClass->New());
  //auto collBuffers = collection->getBuffers();
  //// If we have a valid data buffer class we know that have to read data,
  //// otherwise we are handling a subset collection
  //if (theClass) {
  //  collBuffers.data = theClass->New();
  //} else {
  //  collection->setSubsetCollection();
  //}

  //const auto localEntry = m_chain->LoadTree(m_eventNumber);
  //// After switching trees in the chain, branch pointers get invalidated so
  //// they need to be reassigned.
  //// NOTE: root 6.22/06 requires that we get completely new branches here,
  //// with 6.20/04 we could just re-set them
  //if (localEntry == 0) {
  //  branches.data = root_utils::getBranch(m_chain, name.c_str());

  //  // reference collections
  //  if (auto* refCollections = collBuffers.references) {
  //    for (size_t i = 0; i < refCollections->size(); ++i) {
  //      const auto brName = root_utils::refBranch(name, i);
  //      branches.refs[i] = root_utils::getBranch(m_chain, brName.c_str());
  //    }
  //  }

  //  // vector members
  //  if (auto* vecMembers = collBuffers.vectorMembers) {
  //    for (size_t i = 0; i < vecMembers->size(); ++i) {
  //      const auto brName = root_utils::vecBranch(name, i);
  //      branches.vecs[i] = root_utils::getBranch(m_chain, brName.c_str());
  //    }
  //  }
  //}

  //// set the addresses
  //root_utils::setCollectionAddresses(collection, branches);

  //return readCollectionData(branches, collection, localEntry, name);
  return nullptr;
}

CollectionBase* ROOTNTupleReader::readCollectionData(const root_utils::CollectionBranches& branches,
                                               CollectionBase* collection, Long64_t entry, const std::string& name) {
  // Read all data
  //if (branches.data) {
  //  branches.data->GetEntry(entry);
  //}
  //for (auto* br : branches.refs) {
  //  br->GetEntry(entry);
  //}
  //for (auto* br : branches.vecs) {
  //  br->GetEntry(entry);
  //}

  //// do the unpacking
  //const auto id = m_table->collectionID(name);
  //collection->setID(id);
  //collection->prepareAfterRead();

  //m_inputs.emplace_back(std::make_pair(collection, name));
  //return collection;
  return nullptr;
}

void ROOTNTupleReader::openFile(const std::string& filename) {
  //openFiles({filename});
}

void ROOTNTupleReader::openFiles(const std::vector<std::string>& filenames) {
  //m_chain = new TChain("events");
  //for (const auto& filename : filenames) {
  //  m_chain->Add(filename.c_str());
  //}

  //// read the meta data and build the collectionBranches cache
  //// NOTE: This is a small pessimization, if we do not read all collections
  //// afterwards, but it makes the handling much easier in general
  //auto metadatatree = static_cast<TTree*>(m_chain->GetFile()->Get("metadata"));
  //m_table = new CollectionIDTable();
  //metadatatree->SetBranchAddress("CollectionIDs", &m_table);

  //podio::version::Version* versionPtr{nullptr};
  //if (auto* versionBranch = root_utils::getBranch(metadatatree, "PodioVersion")) {
  //  versionBranch->SetAddress(&versionPtr);
  //}

  //// Check if the CollectionTypeInfo branch is there and assume that the file
  //// has been written with with podio pre #197 (<0.13.1) if that is not the case
  //if (auto* collInfoBranch = root_utils::getBranch(metadatatree, "CollectionTypeInfo")) {
  //  auto collectionInfo = new std::vector<root_utils::CollectionInfoT>;
  //  collInfoBranch->SetAddress(&collectionInfo);
  //  metadatatree->GetEntry(0);
  //  createCollectionBranches(*collectionInfo);
  //  delete collectionInfo;
  //} else {
  //  std::cout << "PODIO: Reconstructing CollectionTypeInfo branch from other sources in file: \'"
  //            << m_chain->GetFile()->GetName() << "\'" << std::endl;
  //  metadatatree->GetEntry(0);
  //  const auto collectionInfo = root_utils::reconstructCollectionInfo(m_chain, *m_table);
  //  createCollectionBranches(collectionInfo);
  //}

  //m_fileVersion = versionPtr ? *versionPtr : podio::version::Version{0, 0, 0};
  //delete versionPtr;
}

void ROOTNTupleReader::closeFile() {
  //closeFiles();
}

void ROOTNTupleReader::closeFiles() {
  //delete m_chain;
}

void ROOTNTupleReader::readEvent() {
  //m_chain->GetEntry(m_eventNumber);
  //// first prepare all collections in memory...
  //for (auto inputs : m_inputs) {
  //  inputs.first->prepareAfterRead();
  //}
  //// ...then clean-up the references between them
  //    for(auto inputs : m_inputs){
  //    inputs.first->setReferences(m_registry);

  //  }
}
bool ROOTNTupleReader::isValid() const {
  //return m_chain->GetFile()->IsOpen() && !m_chain->GetFile()->IsZombie();
  return true;
}

void ROOTNTupleReader::endOfEvent() {
  //++m_eventNumber;
  //m_inputs.clear();
}

unsigned ROOTNTupleReader::getEntries() const {
  //return m_chain->GetEntries();
}

void ROOTNTupleReader::goToEvent(unsigned eventNumber) {
  //m_eventNumber = eventNumber;
  //m_inputs.clear();
}

void ROOTNTupleReader::createCollectionBranches(const std::vector<root_utils::CollectionInfoT>& collInfo) {
  //size_t collectionIndex{0};

  //for (const auto& [collID, collType, isSubsetColl] : collInfo) {
  //  // We only write collections that are in the collectionIDTable, so no need
  //  // to check here
  //  const auto name = m_table->name(collID);

  //  root_utils::CollectionBranches branches{};
  //  const auto collectionClass = TClass::GetClass(collType.c_str());

  //  // Need the collection here to setup all the branches. Have to manage the
  //  // temporary collection ourselves
  //  auto collection =
  //      std::unique_ptr<podio::CollectionBase>(static_cast<podio::CollectionBase*>(collectionClass->New()));
  //  collection->setSubsetCollection(isSubsetColl);

  //  if (!isSubsetColl) {
  //    // This branch is guaranteed to exist since only collections that are
  //    // also written to file are in the info metadata that we work with here
  //    branches.data = root_utils::getBranch(m_chain, name.c_str());
  //  }

  //  const auto buffers = collection->getBuffers();
  //  for (size_t i = 0; i < buffers.references->size(); ++i) {
  //    const auto brName = root_utils::refBranch(name, i);
  //    branches.refs.push_back(root_utils::getBranch(m_chain, brName.c_str()));
  //  }

  //  for (size_t i = 0; i < buffers.vectorMembers->size(); ++i) {
  //    const auto brName = root_utils::vecBranch(name, i);
  //    branches.vecs.push_back(root_utils::getBranch(m_chain, brName.c_str()));
  //  }

  //  const std::string bufferClassName = "std::vector<" + collection->getDataTypeName() + ">";
  //  const auto bufferClass = isSubsetColl ? nullptr : TClass::GetClass(bufferClassName.c_str());

  //  m_storedClasses.emplace(name, std::make_tuple(bufferClass, collectionClass, collectionIndex++));
  //  m_collectionBranches.push_back(branches);
  //}
}

} // namespace podio
