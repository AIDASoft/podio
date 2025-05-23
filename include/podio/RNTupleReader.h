#ifndef PODIO_RNTUPLEREADER_H
#define PODIO_RNTUPLEREADER_H

#include "podio/ROOTFrameData.h"
#include "podio/podioVersion.h"
#include "podio/utilities/DatamodelRegistryIOHelpers.h"
#include "podio/utilities/RootHelpers.h"

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleReader.hxx>
#include <RVersion.h>

namespace podio {

/// Introduce a new namespace instead of potentially opening and polluting the
/// ROOT namespace
namespace root_compat {
#if ROOT_VERSION_CODE < ROOT_VERSION(6, 35, 0)
  using RNTupleReader = ROOT::Experimental::RNTupleReader;
#else
  using RNTupleReader = ROOT::RNTupleReader;
#endif
} // namespace root_compat

/// The RNTupleReader can be used to read files that have been written with the
/// RNTuple backend.
///
/// The RNTupleReader provides the data as ROOTFrameData from which a podio::Frame
/// can be constructed. It can be used to read files written by the RNTupleWriter.
class RNTupleReader {

public:
  /// Create a RNTupleReader
  RNTupleReader() = default;
  /// Destructor
  ~RNTupleReader() = default;
  /// The RNTupleReader is not copy-able
  RNTupleReader(const RNTupleReader&) = delete;
  /// The RNTupleReader is not copy-able
  RNTupleReader& operator=(const RNTupleReader&) = delete;

  /// Open a single file for reading.
  ///
  /// @param filename The name of the input file
  void openFile(const std::string& filename);

  /// Open multiple files for reading and then treat them as if they are one file
  ///
  /// @note All of the files are assumed to have the same structure. Specifically
  /// this means:
  /// - The same categories are available from all files
  /// - The collections that are contained in the individual categories are the
  ///   same across all files
  /// - This usually boils down to "the files have been written with the same
  ///   "settings", e.g. they are outputs of a batched process.
  ///
  /// @param filenames The filenames of all input files that should be read
  void openFiles(const std::vector<std::string>& filenames);

  /// Read the next data entry for a given category.
  ///
  /// @param name The category name for which to read the next entry
  /// @param collsToRead (optional) the collection names that should be read. If
  ///             not provided (or empty) all collections will be read
  ///
  /// @returns FrameData from which a podio::Frame can be constructed if the
  ///          category exists and if there are still entries left to read.
  ///          Otherwise a nullptr
  ///
  /// @throws std::invalid_argument in case collsToRead contains collection
  /// names that are not available
  std::unique_ptr<podio::ROOTFrameData> readNextEntry(const std::string& name,
                                                      const std::vector<std::string>& collsToRead = {});

  /// Read the desired data entry for a given category.
  ///
  /// @param name  The category name for which to read the next entry
  /// @param entry The entry number to read
  /// @param collsToRead (optional) the collection names that should be read. If
  ///             not provided (or empty) all collections will be read
  ///
  /// @returns FrameData from which a podio::Frame can be constructed if the
  ///          category and the desired entry exist. Otherwise a nullptr
  ///
  /// @throws std::invalid_argument in case collsToRead contains collection
  /// names that are not available
  std::unique_ptr<podio::ROOTFrameData> readEntry(const std::string& name, const unsigned entry,
                                                  const std::vector<std::string>& collsToRead = {});

  /// Get the names of all the available Frame categories in the current file(s).
  ///
  /// @returns The names of the available categores from the file
  std::vector<std::string_view> getAvailableCategories() const;

  /// Get the number of entries for the given name
  ///
  /// @param name The name of the category
  ///
  /// @returns The number of entries that are available for the category
  unsigned getEntries(const std::string& name);

  /// Get the build version of podio that has been used to write the current
  /// file
  ///
  /// @returns The podio build version
  podio::version::Version currentFileVersion() const {
    return m_fileVersion;
  }

  /// Get the (build) version of a datamodel that has been used to write the
  /// current file
  ///
  /// @param name The name of the datamodel
  ///
  /// @returns The (build) version of the datamodel if available or an empty
  ///          optional
  std::optional<podio::version::Version> currentFileVersion(const std::string& name) const {
    return m_datamodelHolder.getDatamodelVersion(name);
  }

  /// Get the datamodel definition for the given name
  ///
  /// @param name The name of the datamodel
  ///
  /// @returns The high level definition of the datamodel in JSON format
  const std::string_view getDatamodelDefinition(const std::string& name) const {
    return m_datamodelHolder.getDatamodelDefinition(name);
  }

  /// Get all names of the datamodels that are available from this reader
  ///
  /// @returns The names of the datamodels
  std::vector<std::string> getAvailableDatamodels() const {
    return m_datamodelHolder.getAvailableDatamodels();
  }

private:
  /**
   * Initialize the given category by filling the maps with metadata information
   * that will be used later
   */
  bool initCategory(const std::string& category);

  /**
   * Read and reconstruct the generic parameters of the Frame
   */
  GenericParameters readEventMetaData(const std::string& name, const unsigned localEntry, const unsigned readerIndex);

  template <typename T>
  void readParams(const std::string& name, const unsigned entNum, const unsigned readerIndex,
                  GenericParameters& params);

  std::unique_ptr<root_compat::RNTupleReader> m_metadata{};

  podio::version::Version m_fileVersion{};
  DatamodelDefinitionHolder m_datamodelHolder{};

  std::unordered_map<std::string, std::vector<std::unique_ptr<root_compat::RNTupleReader>>> m_readers{};
  std::unordered_map<std::string, std::unique_ptr<root_compat::RNTupleReader>> m_metadata_readers{};
  std::vector<std::string> m_filenames{};

  std::unordered_map<std::string, unsigned> m_entries{};
  // Map category to a vector that contains at how many entries each reader starts
  // For example, if we have 3 readers and the first one has 10 entries, the second one 20 and the third one 30
  // then the vector will be {0, 10, 30}
  // 60 is not needed because anything after 30 will be in the last reader
  std::unordered_map<std::string, std::vector<unsigned>> m_readerEntries{};
  std::unordered_map<std::string, unsigned> m_totalEntries{};

  /// Map each category to the collections that have been written and are available
  std::unordered_map<std::string, std::vector<podio::root_utils::CollectionWriteInfo>> m_collectionInfo{};

  std::vector<std::string> m_availableCategories{};

  std::unordered_map<std::string, std::shared_ptr<podio::CollectionIDTable>> m_idTables{};
};

} // namespace podio

#endif
