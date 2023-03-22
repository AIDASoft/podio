#ifndef PODIO_ROOTNTUPLEREADER_H
#define PODIO_ROOTNTUPLEREADER_H

#include "podio/ROOTFrameData.h"
#include "podio/CollectionBranches.h"
#include "podio/ICollectionProvider.h"
#include "podio/podioVersion.h"
#include "podio/utilities/DatamodelRegistryIOHelpers.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleModel.hxx>


namespace rnt = ROOT::Experimental;
// forward declarations
class TClass;
class TFile;

namespace podio {

namespace detail {
  // Information about the data vector as wall as the collection class type
  // and the index in the collection branches cache vector
  using CollectionInfo = std::tuple<const TClass*, const TClass*, size_t>;

} // namespace detail

class EventStore;
class CollectionBase;
class Registry;
class CollectionIDTable;
class GenericParameters;
/**
This class has the function to read available data from disk
and to prepare collections and buffers.
**/
class ROOTNTupleReader{
  friend EventStore;

public:
  ROOTNTupleReader() = default;
  ~ROOTNTupleReader() = default;

  // non-copyable
  ROOTNTupleReader(const ROOTNTupleReader&) = delete;
  ROOTNTupleReader& operator=(const ROOTNTupleReader&) = delete;

  void openFile(const std::string& filename);
  void openFiles(const std::vector<std::string>& filename);

  /**
   * Read the next data entry from which a Frame can be constructed for the
   * given name. In case there are no more entries left for this name or in
   * case there is no data for this name, this returns a nullptr.
   */
  std::unique_ptr<podio::ROOTFrameData> readNextEntry(const std::string& name);

  /**
   * Read the specified data entry from which a Frame can be constructed for
   * the given name. In case the entry does not exist for this name or in case
   * there is no data for this name, this returns a nullptr.
   */
  std::unique_ptr<podio::ROOTFrameData> readEntry(const std::string& name, const unsigned entry);

  /// Returns number of entries for the given name
  unsigned getEntries(const std::string& name);

  /// Get the build version of podio that has been used to write the current file
  podio::version::Version currentFileVersion() const {
    return m_fileVersion;
  }

  void closeFile();

private:
  std::vector<std::unique_ptr<ROOTNTupleReader>> m_files;
  std::unique_ptr<ROOT::Experimental::RNTupleReader> m_file;
  std::unique_ptr<ROOT::Experimental::RNTupleReader> m_metadata;

  
private:

  /**
   * Initialze the passed CategoryInfo by setting up the necessary branches,
   * collection infos and all necessary meta data to be able to read entries
   * with this name
   */
  bool initCategory(const std::string& category);

  GenericParameters readEventMetaData(const std::string& name);

  
  podio::version::Version m_fileVersion{0, 0, 0};
  DatamodelDefinitionHolder m_datamodelHolder{};

  std::map<std::string, std::vector<std::unique_ptr<ROOT::Experimental::RNTupleReader>>> m_readers;
  std::map<std::string, std::unique_ptr<ROOT::Experimental::RNTupleReader>> m_metadata_readers;
  std::vector<std::string> m_filenames;
  std::map<std::string, int> m_entries;

  std::map<std::string, std::vector<int>> m_collectionId;
  std::map<std::string, std::vector<std::string>> m_collectionName;
  std::map<std::string, std::vector<std::string>> m_collectionType;
  std::map<std::string, std::vector<bool>> m_isSubsetCollection;

  std::map<std::string, int> m_totalEntries;
  std::vector<std::string> m_availableCategories;

};

} // namespace podio

#endif
