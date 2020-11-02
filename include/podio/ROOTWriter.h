#ifndef ROOTWRITER_H
#define ROOTWRITER_H

#include "podio/CollectionBase.h"
#include "podio/EventStore.h"

#include <string>
#include <vector>
#include <iostream>
#include <string_view>
#include <utility>

// forward declarations
class TFile;
class TTree;

namespace podio {
  class ROOTWriter {

  public:
    ROOTWriter(const std::string& filename, EventStore* store);
    ~ROOTWriter();

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
    GenericParameters* m_evtMD ;
    std::vector<CollectionBase*> m_storedCollections;
    std::vector<std::string> m_collectionsToWrite;
    bool m_firstEvent{true};
  };

} //namespace
#endif
