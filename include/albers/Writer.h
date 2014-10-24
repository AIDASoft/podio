#ifndef ALBERS_WRITER_H
#define ALBERS_WRITER_H

#include <string>
#include <vector>

// ROOT specific includes
#include "TTree.h"

// forward declarations
class TFile;

namespace albers {
  class CollectionBase;
  class Registry;

  class Writer {

  public:
    Writer(const std::string& filename, Registry* registry);
    ~Writer();

    template<typename T>
    void registerForWrite(const std::string& name, T& coll);
    void writeEvent();
    void finish();

  private:
    // members
    std::string m_filename;
    Registry* m_registry;
    TFile* m_file;
    TTree* m_datatree;
    TTree* m_metadatatree;
    std::vector<CollectionBase*> m_storedCollections; // preserving the order is important!
  
  };

template<typename T>
  void Writer::registerForWrite(const std::string& name, T& coll){
    m_datatree->Branch(name.c_str(), coll._getBuffer());
    m_storedCollections.emplace_back(&coll);
    // TODO: register the aux collections as well!
  }

} //namespace
#endif
