#include "rootUtils.h"

#include "podio/CollectionBase.h"
#include "podio/EventStore.h"
#include "podio/ROOTWriter.h"

// ROOT specifc includes
#include "TFile.h"
#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleModel.hxx>

#include "podio/ROOTNTupleWriter.h"


namespace podio {

  ROOTNTupleWriter::ROOTNTupleWriter(const std::string& filename, EventStore* store) :
    m_events(nullptr),
    m_metadata(nullptr),
    m_runMD(nullptr),
    m_colMD(nullptr),
    m_evtMD(nullptr),
    m_file(new TFile(filename.c_str(),"RECREATE","data file")),
    m_store(store)
  {
    m_events = rnt::RNTupleModel::Create();
    m_metadata = rnt::RNTupleModel::Create();
    m_runMD = rnt::RNTupleModel::Create();
    m_colMD = rnt::RNTupleModel::Create();
    m_evtMD = rnt::RNTupleModel::Create();

  }

  ROOTNTupleWriter::~ROOTNTupleWriter() {
    delete m_file;
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

  }

  void ROOTNTupleWriter::finish() {

  }

} //namespace podio
