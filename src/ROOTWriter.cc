#include "rootUtils.h"

// podio specific includes
#include "podio/CollectionBase.h"
#include "podio/EventStore.h"
#include "podio/ROOTWriter.h"
#include "podio/podioVersion.h"

// ROOT specifc includes
#include "TFile.h"
#include "TTree.h"

namespace podio {
ROOTWriter::ROOTWriter(const std::string& filename, EventStore* store) :
    m_filename(filename),
    m_store(store),
    m_file(new TFile(filename.c_str(), "RECREATE", "data file")),
    m_datatree(new TTree("events", "Events tree")),
    m_metadatatree(new TTree("metadata", "Metadata tree")),
    m_runMDtree(new TTree("run_metadata", "Run metadata tree")),
    m_evtMDtree(new TTree("evt_metadata", "Event metadata tree")),
    m_colMDtree(new TTree("col_metadata", "Collection metadata tree")) {

  m_evtMDtree->Branch("evtMD", "GenericParameters", m_store->eventMetaDataPtr());
}

ROOTWriter::~ROOTWriter() {
  delete m_file;
}

void ROOTWriter::writeEvent() {
  std::vector<StoreCollection> collections;
  collections.reserve(m_collectionsToWrite.size());
  for (const auto& name : m_collectionsToWrite) {
    const podio::CollectionBase* coll;
    m_store->get(name, coll);
    collections.emplace_back(name, const_cast<podio::CollectionBase*>(coll));
    collections.back().second->prepareForWrite();
  }

  if (m_firstEvent) {
    createBranches(collections);
    m_firstEvent = false;
  } else {
    setBranches(collections);
  }

  m_datatree->Fill();
  m_evtMDtree->Fill();
}

void ROOTWriter::createBranches(const std::vector<StoreCollection>& collections) {
  for (auto& [name, coll] : collections) {
    root_utils::CollectionBranches branches;
    const auto collBuffers = coll->getBuffers();
    if (collBuffers.data) {
      // only create the data buffer branch if necessary

      auto collClassName = "vector<" + coll->getDataTypeName() + ">";

      branches.data = m_datatree->Branch(name.c_str(), collClassName.c_str(), collBuffers.data);
    }

    // reference collections
    if (auto refColls = collBuffers.references) {
      int i = 0;
      for (auto& c : (*refColls)) {
        const auto brName = root_utils::refBranch(name, i);
        branches.refs.push_back(m_datatree->Branch(brName.c_str(), c.get()));
        ++i;
      }
    }

    // vector members
    if (auto vminfo = collBuffers.vectorMembers) {
      int i = 0;
      for (auto& [type, vec] : (*vminfo)) {
        const auto typeName = "vector<" + type + ">";
        const auto brName = root_utils::vecBranch(name, i);
        branches.vecs.push_back(m_datatree->Branch(brName.c_str(), typeName.c_str(), vec));
        ++i;
      }
    }

    m_collectionBranches.push_back(branches);
  }
}

void ROOTWriter::setBranches(const std::vector<StoreCollection>& collections) {
  size_t iCollection = 0;
  for (auto& coll : collections) {
    const auto& branches = m_collectionBranches[iCollection];
    root_utils::setCollectionAddresses(coll.second->getBuffers(), branches);

    iCollection++;
  }
}

void ROOTWriter::finish() {
  // now we want to safe the metadata. This includes info about the
  // collections
  const auto collIDTable = m_store->getCollectionIDTable();
  m_metadatatree->Branch("CollectionIDs", collIDTable);

  // collectionID, collection type, subset collection
  std::vector<root_utils::CollectionInfoT> collectionInfo;
  collectionInfo.reserve(m_collectionsToWrite.size());
  for (const auto& name : m_collectionsToWrite) {
    const auto collID = collIDTable->collectionID(name);
    const podio::CollectionBase* coll{nullptr};
    // No check necessary, only registered collections possible
    m_store->get(name, coll);
    const auto collType = coll->getTypeName();
    // const auto collType = "std::vector<" + coll->getDataTypeName() + ">";
    collectionInfo.emplace_back(collID, std::move(collType), coll->isSubsetCollection());
  }

  m_metadatatree->Branch("CollectionTypeInfo", &collectionInfo);

  podio::version::Version podioVersion = podio::version::build_version;
  m_metadatatree->Branch("PodioVersion", &podioVersion);

  m_metadatatree->Fill();

  m_colMDtree->Branch("colMD", "std::map<int,podio::GenericParameters>", m_store->getColMetaDataMap());
  m_colMDtree->Fill();
  m_runMDtree->Branch("runMD", "std::map<int,podio::GenericParameters>", m_store->getRunMetaDataMap());
  m_runMDtree->Fill();

  m_file->Write();
  m_file->Close();
}

bool ROOTWriter::registerForWrite(const std::string& name) {
  const podio::CollectionBase* tmp_coll(nullptr);
  if (!m_store->get(name, tmp_coll)) {
    std::cerr << "no such collection to write, throw exception." << std::endl;
    return false;
  }

  m_collectionsToWrite.push_back(name);
  return true;
}

} // namespace podio
