#ifndef ROOTREADER_H
#define ROOTREADER_H

#include <algorithm>
#include <string>
#include <vector>

// forward declarations
class TFile;
class TTree;

#include <iostream>

#include "podio/ICollectionProvider.h"
#include "podio/IReader.h"
/*

This class has the function to read available data from disk
and to prepare collections and buffers.

 */


namespace podio {

class EventStore;
class CollectionBase;
class Registry;
class CollectionIDTable;

class ROOTReader : public IReader {
  friend EventStore;
  public:
    ROOTReader() : m_eventNumber(0) {}
    ~ROOTReader();
    void openFile(const std::string& filename);
    void closeFile(){};

    /// Read all collections requested
    void readEvent();

    /// get collection of name/type; returns true if successfull
    template<typename T>
    bool getCollection(const std::string& name, T*& collection);

    /// Read CollectionIDTable from ROOT file
    CollectionIDTable* getCollectionIDTable() {return m_table;}

    /// Returns number of entries in the TTree
    unsigned getEntries() const;

    /// Preparing to read next event
    void endOfEvent();

    /// Preparing to read a given event
    void goToEvent(unsigned evnum);

  private:

    void readCollectionIDTable();

    /// Implementation for collection reading
    CollectionBase* readCollection(const std::string& name);

  private:
    typedef std::pair<CollectionBase*, std::string> Input;
    std::vector<Input> m_inputs;
    CollectionIDTable* m_table;
    TFile* m_file;
    TTree* m_eventTree;
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
