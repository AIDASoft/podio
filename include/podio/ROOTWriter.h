#ifndef ROOTWRITER_H
#define ROOTWRITER_H

#include "podio/EventStore.h"

#include <string>
#include <vector>
#include <iostream>

// ROOT specific includes
#include "TTree.h"

// forward declarations
class TFile;

namespace podio {
  class CollectionBase;
  class Registry;

  class ROOTWriter {

  public:
    ROOTWriter(const std::string& filename, EventStore* store);
    ~ROOTWriter();

    template<typename T>
    void registerForWrite(const std::string& name);
    void writeEvent();
    void finish();

  private:
    // members
    std::string m_filename;
    EventStore* m_store;
    TFile* m_file;
    TTree* m_datatree;
    TTree* m_metadatatree;
    std::vector<CollectionBase*> m_storedCollections;

  };

template<typename T>
  void ROOTWriter::registerForWrite(const std::string& name){
    const T* tmp_coll(nullptr);
    m_store->get(name, tmp_coll);
    T* coll = const_cast<T*>(tmp_coll);

    if(coll==nullptr) {
      std::cerr<<"no such collection to write, throw exception."<<std::endl;
    }
    else {
      m_datatree->Branch(name.c_str(), coll->_getBuffer());
      auto colls = coll->referenceCollections();
      if (colls != nullptr){
      int i = 0;
      for(auto& c : (*colls)){
        m_datatree->Branch((name+"#"+std::to_string(i)).c_str(),c);
        ++i;
      }
    }
      m_storedCollections.emplace_back(coll);
    }
  }

} //namespace
#endif
