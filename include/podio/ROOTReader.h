#ifndef PODIO_ROOTREADER_H
#define PODIO_ROOTREADER_H

#include "podio/CollectionBranches.h"
#include "podio/IReader.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

// forward declarations
class TClass;
class TFile;
class TTree;
class TChain;

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
  // todo: see https://github.com/AIDASoft/podio/issues/290
  ~ROOTReader(); // NOLINT(modernize-use-equals-default)

  // non-copyable
  ROOTReader(const ROOTReader&) = delete;
  ROOTReader& operator=(const ROOTReader&) = delete;

  void openFile(const std::string& filename) override;
  void openFiles(const std::vector<std::string>& filenames);
  void closeFile() override;
  void closeFiles();

  /// Read all collections requested
  void readEvent() override;

  /// Read CollectionIDTable from ROOT file
  std::shared_ptr<CollectionIDTable> getCollectionIDTable() override {
    return m_table;
  }

  /// Returns number of entries in the TTree
  unsigned getEntries() const override;

  /// Preparing to read next event
  void endOfEvent() override;

  /// Preparing to read a given event
  void goToEvent(unsigned evnum) override;

  podio::version::Version currentFileVersion() const override {
    return m_fileVersion;
  }

  /// Check if TFile is valid
  bool isValid() const override;

private:
  /// Implementation for collection reading
  CollectionBase* readCollection(const std::string& name) override;

  /// read event meta data for current event
  GenericParameters* readEventMetaData() override;

  /// read the collection meta data
  std::map<int, GenericParameters>* readCollectionMetaData() override;

  /// read the run meta data
  std::map<int, GenericParameters>* readRunMetaData() override;

private:
  void createCollectionBranches(const std::vector<std::tuple<int, std::string, bool, std::string>>& collInfo);

  std::pair<TTree*, unsigned> getLocalTreeAndEntry(const std::string& treename);
  // Information about the data vector as wall as the collection class type
  // and the index in the collection branches cache vector
  using CollectionInfo = std::tuple<const TClass*, const TClass*, size_t>;

  CollectionBase* getCollection(const std::pair<std::string, CollectionInfo>& collInfo);
  CollectionBase* readCollectionData(const root_utils::CollectionBranches& branches, CollectionBase* collection,
                                     Long64_t entry, const std::string& name);

  // cache collections that have been read already in a given event
  typedef std::pair<CollectionBase*, std::string> Input;
  std::vector<Input> m_inputs{};

  // cache the necessary information to more quickly construct and read each
  // collection after it has been read the very first time
  std::map<std::string, CollectionInfo> m_storedClasses{};

  std::shared_ptr<CollectionIDTable> m_table{nullptr};
  TChain* m_chain{nullptr};
  unsigned m_eventNumber{0};

  // Similar to writing we cache the branches that belong to each collection
  // in order to not having to look them up every event. However, for the
  // reader we cannot guarantee a fixed order of collections as they are read
  // on demand. Hence, we give each collection an index the first time it is
  // read and we start caching the branches.
  size_t m_collectionIndex = 0;
  std::vector<root_utils::CollectionBranches> m_collectionBranches{};

  podio::version::Version m_fileVersion{0, 0, 0};
};

} // namespace podio

#endif
