#ifndef ROOTREADER_H
#define ROOTREADER_H

#include "podio/rootUtils.h"

#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <utility>
#include <tuple>

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
    ROOTReader() = default;
    ~ROOTReader();

    //non-copyable
    ROOTReader(const ROOTReader &) = delete;
    ROOTReader& operator=(const ROOTReader &) = delete;

    void openFile(const std::string& filename) override;
    void openFiles(const std::vector<std::string>& filenames);
    void closeFile() override;
    void closeFiles();

    /// Read all collections requested
    void readEvent();

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
    using CollectionInfo = std::tuple<const TClass*, const TClass*, size_t>;

    CollectionBase* getCollection(const std::pair<std::string, CollectionInfo>& collInfo);
    CollectionBase* readCollectionData(const root_utils::CollectionBranches& branches, CollectionBase* collection, Long64_t entry, const std::string& name);

    typedef std::pair<CollectionBase*, std::string> Input;
    std::vector<Input> m_inputs{};

    std::map<std::string, CollectionInfo> m_storedClasses{};
    CollectionIDTable* m_table{nullptr};
    TChain* m_chain{nullptr};
    unsigned m_eventNumber{0};

    size_t m_collectionIndex = 0;
    std::vector<root_utils::CollectionBranches> m_collectionBranches{};
};

} // namespace

#endif
