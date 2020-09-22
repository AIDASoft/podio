#ifndef ROOTREADER_H
#define ROOTREADER_H

#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <utility>

// forward declarations
class TClass;
class TFile;
class TTree;
class TChain;


#include "podio/ICollectionProvider.h"
#include "podio/IReader.h"


namespace podio {

class EventStore;
class CollectionBase;
class Registry;
class CollectionIDTable;
class GenericParameters;
/**
This class has the function to read available data from disk
and to prepare collections and buffers.
**/
class ROOTReader : public IReader {
  friend EventStore;
  public:
    ROOTReader() : m_eventNumber(0) {}
    ~ROOTReader();
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
    CollectionIDTable* getCollectionIDTable() override final {return m_table;}

    /// Returns number of entries in the TTree
    unsigned getEntries() const override;

    /// Preparing to read next event
    void endOfEvent() override;

    /// Preparing to read a given event
    void goToEvent(unsigned evnum);

    /// Check if TFile is valid
    virtual bool isValid() const override final;

  private:

    /// Implementation for collection reading
    CollectionBase* readCollection(const std::string& name) override final;

    /// read event meta data for current event
    GenericParameters* readEventMetaData() override final ;

  /// read the collection meta data
    std::map<int,GenericParameters>* readCollectionMetaData() override final ;

  /// read the run meta data
    std::map<int,GenericParameters>* readRunMetaData() override final ;

  private:
    std::pair<TTree*, unsigned> getLocalTreeAndEntry(const std::string& treename);

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
