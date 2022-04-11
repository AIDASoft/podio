#ifndef PODIO_ROOTFRAMEREADER_H
#define PODIO_ROOTFRAMEREADER_H

#include "podio/CollectionBranches.h"
#include "podio/ROOTRawData.h"
#include "podio/podioVersion.h"

#include "TChain.h"

#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

// forward declarations
class TClass;
// class TChain;
class TFile;
class TTree;

namespace podio {

class EventStore;
class CollectionBase;
class CollectionIDTable;
class GenericParameters;
struct CollectionBuffers;

/**
This class has the function to read available data from disk
and to prepare collections and buffers.
**/
class ROOTFrameReader {

public:
  ROOTFrameReader() = default;
  ~ROOTFrameReader() = default;

  // non-copyable
  ROOTFrameReader(const ROOTFrameReader&) = delete;
  ROOTFrameReader& operator=(const ROOTFrameReader&) = delete;

  void openFile(const std::string& filename);

  void openFiles(const std::vector<std::string>& filenames);

  /// Read all collections requested
  std::unique_ptr<podio::ROOTRawData> readNextEvent();

  /// Returns number of entries in the TTree
  unsigned getEntries() const;

  podio::version::Version currentFileVersion() const {
    return m_fileVersion;
  }

private:
  void createCollectionBranches(const std::vector<std::tuple<int, std::string, bool>>& collInfo);

  // Information about the data vector as wall as the collection class type
  // and the index in the collection branches cache vector
  using CollectionInfo = std::tuple<const TClass*, const TClass*, size_t>;

  podio::CollectionBuffers getCollectionBuffers(const std::pair<std::string, CollectionInfo>& collInfo);

  // cache the necessary information to more quickly construct and read each
  // collection after it has been read the very first time
  std::vector<std::pair<std::string, CollectionInfo>> m_storedClasses{};

  std::shared_ptr<CollectionIDTable> m_table{nullptr};
  std::unique_ptr<TChain> m_chain{nullptr};
  unsigned m_eventNumber{0};

  // Similar to writing we cache the branches that belong to each collection
  // in order to not having to look them up every event. However, for the
  // reader we cannot guarantee a fixed order of collections as they are read
  // on demand. Hence, we give each collection an index the first time it is
  // read and we start caching the branches.
  std::vector<root_utils::CollectionBranches> m_collectionBranches{};

  podio::version::Version m_fileVersion{0, 0, 0};
};

} // namespace podio

#endif // PODIO_ROOTFRAMEREADER_H
