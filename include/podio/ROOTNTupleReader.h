#ifndef PODIO_ROOTNTUPLEREADER_H
#define PODIO_ROOTNTUPLEREADER_H

#include "podio/ROOTFrameData.h"
#include "podio/CollectionBranches.h"
#include "podio/ICollectionProvider.h"
#include "podio/podioVersion.h"
#include "podio/utilities/DatamodelRegistryIOHelpers.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleModel.hxx>


namespace podio {

/**
This class has the function to read available data from disk
and to prepare collections and buffers.
**/
class ROOTNTupleReader{

public:
  ROOTNTupleReader() = default;
  ~ROOTNTupleReader() = default;

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
  /**
   * Initialize the given category by filling the maps with metadata information
   * that will be used later
   */
  bool initCategory(const std::string& category);

  /**
   * Read and reconstruct the generic parameters of the Frame
   */
  GenericParameters readEventMetaData(const std::string& name, unsigned entNum);

  std::unique_ptr<ROOT::Experimental::RNTupleReader> m_metadata{};

  podio::version::Version m_fileVersion{};
  DatamodelDefinitionHolder m_datamodelHolder{};

  std::map<std::string, std::vector<std::unique_ptr<ROOT::Experimental::RNTupleReader>>> m_readers{};
  std::map<std::string, std::unique_ptr<ROOT::Experimental::RNTupleReader>> m_metadata_readers{};
  std::vector<std::string> m_filenames{};

  std::map<std::string, int> m_entries{};
  std::map<std::string, unsigned> m_totalEntries{};

  std::map<std::string, std::vector<int>> m_collectionId{};
  std::map<std::string, std::vector<std::string>> m_collectionName{};
  std::map<std::string, std::vector<std::string>> m_collectionType{};
  std::map<std::string, std::vector<bool>> m_isSubsetCollection{};
  std::vector<std::string> m_availableCategories{};

};

} // namespace podio

#endif
