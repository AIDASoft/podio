#ifndef ROOTREADER_H
#define ROOTREADER_H

#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include <iostream>

// ROOT specific includes
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TTreeCache.h"
#include "TChain.h"


#include "podio/ICollectionProvider.h"
#include "podio/IReader.h"
#include "podio/CollectionIDTable.h"
#include "podio/CollectionBase.h"


namespace podio {

class EventStore;
class Registry;

/**
This class has the function to read available data from disk
and to prepare collections and buffers.
**/
class ROOTReader : public IReader {
  friend EventStore;
  public:
    ROOTReader() : m_eventNumber(0) {}
    virtual ~ROOTReader() {}
    void openFile(const std::string& filename);
    void openFiles(const std::vector<std::string>& filenames);
    void closeFile();
    void closeFiles();

    /// Read all collections requested
    void readEvent();

    /// get collection of name/type; returns true if successfull
    template<typename T>
    bool getCollection(const std::string& name, T*& collection);

    /// Read CollectionIDTable from ROOT file
    virtual CollectionIDTable* getCollectionIDTable() override final {return m_table;}

    /// Returns number of entries in the TTree
    unsigned getEntries() const;

    /// Preparing to read next event
    void endOfEvent();

    /// Preparing to read a given event
    void goToEvent(unsigned evnum);

    /// Check if TFile is valid
    virtual bool isValid() const override final {
      return m_chain->GetFile()->IsOpen() && !m_chain->GetFile()->IsZombie();
    }

  private:

    /// Implementation for collection reading
    virtual CollectionBase* readCollection(const std::string& name) override final{
      // has the collection already been constructed?
      auto p = std::find_if(begin(m_inputs), end(m_inputs),
          [name](ROOTReader::Input t){ return t.second == name;});
      if (p != end(m_inputs)){
        return p->first;
      }
      auto branch = m_chain->GetBranch(name.c_str());
      if (nullptr == branch) {
        return nullptr;
      }
      // look for involved classes
      TClass* theClass(nullptr);
      TClass* collectionClass(nullptr);
      auto result = m_storedClasses.find(name);
      if (result != m_storedClasses.end()) {
        theClass = result->second.first;
        collectionClass = result->second.second;
      } else {
        auto dataClassName = branch->GetClassName();
        theClass = gROOT->GetClass(dataClassName);
        if (theClass == nullptr) return nullptr;
        // now create the transient collections
        // some workaround until gcc supports regex properly:
        auto dataClassString = std::string(dataClassName);
        auto start = dataClassString.find("<");
        auto end   = dataClassString.find(">");
        //getting "TypeCollection" out of "vector<TypeData>"
        auto classname = dataClassString.substr(start+1, end-start-5);
        auto collectionClassName = classname+"Collection";
        collectionClass = gROOT->GetClass(collectionClassName.c_str());
        if (collectionClass == nullptr) return nullptr;
        // cache classes found for future usage
        m_storedClasses[name] = std::pair<TClass*,TClass*>(theClass, collectionClass);
      }
      // now create buffers and collections
      void* buffer = theClass->New();
      CollectionBase* collection = nullptr;
      collection = static_cast<CollectionBase*>(collectionClass->New());
      // connect buffer, collection and branch
      collection->setBuffer(buffer);
      branch->SetAddress(collection->getBufferAddress());
      m_inputs.emplace_back(std::make_pair(collection,name));
      Long64_t localEntry = m_chain->LoadTree(m_eventNumber);
      // After switching trees in the chain, branch pointers get invalidated
      // so they need to be reassigned as well as addresses
      if(localEntry == 0){
          branch = m_chain->GetBranch(name.c_str());
          branch->SetAddress(collection->getBufferAddress());
      }
      branch->GetEntry(localEntry);
      // load the collections containing references
      auto refCollections = collection->referenceCollections();
  
      if (refCollections != nullptr) {
        for (int i = 0, end = refCollections->size(); i!=end; ++i){
          branch = m_chain->GetBranch((name+"#"+std::to_string(i)).c_str());
          branch->SetAddress(&(*refCollections)[i]);
          branch->GetEntry(localEntry);
        }
      }
      // load the collections containing vector members
      auto vecmeminfo = collection->vectorMembers();
      if (vecmeminfo != nullptr) {
        for (int i = 0, end = vecmeminfo->size(); i!=end; ++i){
          branch = m_chain->GetBranch((name+"_"+std::to_string(i)).c_str());
          branch->SetAddress((*vecmeminfo)[i].second);
          branch->GetEntry(localEntry);
        }
      }
      auto id = m_table->collectionID(name);
      collection->setID(id);
      collection->prepareAfterRead();
      return collection;
    }

  private:
    typedef std::pair<CollectionBase*, std::string> Input;
    std::vector<Input> m_inputs;
    std::map<std::string, std::pair<TClass*,TClass*> > m_storedClasses;
    CollectionIDTable* m_table;
    TChain* m_chain;
    unsigned m_eventNumber;
};

template<typename T>
bool ROOTReader::getCollection(const std::string& name, T*& collection){
  collection = dynamic_cast<T*>(readCollection(name));
  if (collection != nullptr) {
    return true;
  } else {
    return false;
  }
}

} // namespace

#endif
