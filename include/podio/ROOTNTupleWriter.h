#ifndef RNTUPLEWRITER_H
#define RNTUPLEWRITER_H

#include "podio/CollectionBase.h"
#include "podio/EventStore.h"
#include "podio/CollectionBranches.h"

#include "TBranch.h"
#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleModel.hxx>

#include <string>
#include <vector>
#include <iostream>
#include <string_view>
#include <utility>
#include <cstdint>


namespace rnt = ROOT::Experimental;
// forward declarations
class TFile;
class TTree;

namespace podio {
  class ROOTNTupleWriter {
  public:
  ROOTNTupleWriter(const std::string& filename, EventStore* store);
  ~ROOTNTupleWriter();

  // TODO: delete copy ctors
  //
  bool registerForWrite(const std::string& name);
  void writeEvent();
  void finish();

  using StoreCollection = std::pair<const std::string&, podio::CollectionBase*>;
  void createModels(const std::vector<StoreCollection>& collections);

  private:
  std::unique_ptr< rnt::RNTupleModel > m_events;
  std::unique_ptr< rnt::RNTupleModel > m_metadata;
  std::unique_ptr< rnt::RNTupleModel > m_runMD;
  std::unique_ptr< rnt::RNTupleModel > m_colMD;
  std::unique_ptr< rnt::RNTupleModel > m_evtMD;

  TFile* m_file;

  std::vector<std::string> m_collectionsToWrite;

  EventStore* m_store;
  bool m_firstEvent;
  std::unique_ptr< rnt::RNTupleWriter > m_ntuple_events;
  std::unique_ptr< rnt::RNTupleWriter > m_ntuple_metadata;
  std::unique_ptr< rnt::RNTupleWriter > m_ntuple_runMD;
  std::unique_ptr< rnt::RNTupleWriter > m_ntuple_colMD;
  std::unique_ptr< rnt::RNTupleWriter > m_ntuple_evtMD;
  };

} //namespace podio

#endif //RNTUPLEWRITER_H

