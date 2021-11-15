#include "rootUtils.h"

#include "podio/CollectionBase.h"
#include "podio/EventStore.h"
#include "podio/ROOTWriter.h"

// ROOT specifc includes
#include "TFile.h"

#include "podio/ROOTNTupleWriter.h"

using StoreCollection = std::pair<const std::string&, podio::CollectionBase*>;


namespace podio {

  ROOTNTupleWriter::ROOTNTupleWriter(const std::string& filename, EventStore* store) :
    m_events(nullptr),
    m_metadata(nullptr),
    m_runMD(nullptr),
    m_colMD(nullptr),
    m_evtMD(nullptr),
    m_file(new TFile(filename.c_str(),"RECREATE","data file")),
    m_collectionsToWrite(),
    m_store(store),
    m_firstEvent(true),
    m_ntuple_events(nullptr),
    m_ntuple_metadata(nullptr),
    m_ntuple_runMD(nullptr),
    m_ntuple_colMD(nullptr),
    m_ntuple_evtMD(nullptr)
  {
    m_events = rnt::RNTupleModel::Create();
    m_metadata = rnt::RNTupleModel::Create();
    m_runMD = rnt::RNTupleModel::Create();
    m_colMD = rnt::RNTupleModel::Create();
    m_evtMD = rnt::RNTupleModel::Create();
  }

  ROOTNTupleWriter::~ROOTNTupleWriter() {
    // rnt::RNTupleWriter deletes file
  }

  bool ROOTNTupleWriter::registerForWrite(const std::string& name) {
    const podio::CollectionBase* tmp_coll(nullptr);
    if (!m_store->get(name, tmp_coll)) {
      std::cerr << "no such collection to write, throw exception." << std::endl;
      return false;
    }
    m_collectionsToWrite.push_back(name);
    return true;
  }


  void ROOTNTupleWriter::writeEvent() {
  // todo: we only have to do this for the first event, right?
  std::vector<StoreCollection> collections;
  collections.reserve(m_collectionsToWrite.size());
  for (const auto& name : m_collectionsToWrite) {
    const podio::CollectionBase* coll;
    m_store->get(name, coll);
    collections.emplace_back(name, const_cast<podio::CollectionBase*>(coll));
    collections.back().second->prepareForWrite();
  }

  if (m_firstEvent) {
    createModels(collections);
    m_firstEvent = false;
  } 

  m_ntuple_events->Fill();
}

void ROOTNTupleWriter::createModels(const std::vector<StoreCollection>& collections) {
  for (auto& [name, coll] : collections) {
    const auto collBuffers = coll->getBuffers();
    if (collBuffers.data) {
      // note: these interfaces may still be simplified on the rntuple side
      auto collClassName = "vector<" + coll->getDataTypeName() +">";
      std::cout << collClassName << std::endl;
      auto field = rnt::Detail::RFieldBase::Create(name,  collClassName).Unwrap();
      // what I cast buffers.data to  does not seem to matter, but it seems I have to cast it to something?
      void* vv =  (void*) *static_cast<std::vector<float>**>(collBuffers.data); 
      m_events->GetDefaultEntry()->CaptureValue(field->CaptureValue(vv));
      m_events->GetFieldZero()->Attach(std::move(field));
    }

    // reference collections
    if (auto refColls = collBuffers.references) {
      int i = 0;
      for (auto& c : (*refColls)) {
        const auto brName = root_utils::refBranch(name, i);
        std::cout << brName << std::endl;
        //TODO:
        ++i;
      }
    }

    // vector members
    if (auto vminfo = collBuffers.vectorMembers) {
      int i = 0;
      for (auto& [type, vec] : (*vminfo)) {
        const auto typeName = "vector<" + type + ">";
        const auto brName = root_utils::vecBranch(name, i);
        //TODO:
        ++i;
      }
    }
  }

    m_ntuple_events = rnt::RNTupleWriter::Append(std::move(m_events), "events", *m_file, {});
    m_ntuple_metadata = rnt::RNTupleWriter::Append(std::move(m_metadata), "metadata", *m_file, {});
}

  void ROOTNTupleWriter::finish() {
  // TODO:
  }

} //namespace podio
