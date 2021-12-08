#ifndef ROOTWRITER_H
#define ROOTWRITER_H

#include "podio/CollectionBase.h"
#include "podio/CollectionBranches.h"
#include "podio/EventStore.h"

#include "TBranch.h"

#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// forward declarations
class TFile;
class TTree;

namespace podio {
class ROOTWriter {

public:
  ROOTWriter(const std::string& filename, EventStore* store);
  ~ROOTWriter();

  // non-copyable
  ROOTWriter(const ROOTWriter&) = delete;
  ROOTWriter& operator=(const ROOTWriter&) = delete;

  bool registerForWrite(const std::string& name);
  void writeEvent();
  void finish();

private:
  using StoreCollection = std::pair<const std::string&, podio::CollectionBase*>;
  void createBranches(const std::vector<StoreCollection>& collections);
  void setBranches(const std::vector<StoreCollection>& collections);

  // members
  std::string m_filename;
  EventStore* m_store;
  TFile* m_file;
  TTree* m_datatree;
  TTree* m_metadatatree;
  TTree* m_runMDtree;
  TTree* m_evtMDtree;
  TTree* m_colMDtree;
  std::vector<std::string> m_collectionsToWrite{};
  // In order to avoid having to look up the branches from the datatree for
  // every event, we cache them in this vector, that is populated the first
  // time we write an event. Since the collections and their order do not
  // change between events, the assocation between the collections to write
  // and their branches is simply index based
  std::vector<root_utils::CollectionBranches> m_collectionBranches{};

  bool m_firstEvent{true};
};

} // namespace podio
#endif
