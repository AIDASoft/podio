#ifndef PODIO_ROOTLAZYREADER_H
#define PODIO_ROOTLAZYREADER_H

#include "podio/ROOTLazyFrameData.h"
#include "podio/podioVersion.h"
#include "podio/utilities/DatamodelRegistryIOHelpers.h"

#include "TChain.h"

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace podio {

class CollectionIDTable;
class GenericParameters;

/// This class reads data from ROOT TTree files lazily: individual collections
/// are only read from disk when they are first accessed via Frame::get().
///
/// The ROOTLazyReader provides data as ROOTLazyFrameData from which a
/// podio::Frame can be constructed. It can be used to read files written by the
/// ROOTWriter. Unlike the ROOTReader which reads all collections eagerly, this
/// reader defers the actual ROOT I/O to the point of collection access.
class ROOTLazyReader {

public:
  ROOTLazyReader() = default;
  ~ROOTLazyReader() = default;

  ROOTLazyReader(const ROOTLazyReader&) = delete;
  ROOTLazyReader& operator=(const ROOTLazyReader&) = delete;
  ROOTLazyReader(ROOTLazyReader&&) = default;
  ROOTLazyReader& operator=(ROOTLazyReader&&) = default;

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
  ///
  /// @param filenames The filenames of all input files that should be read
  void openFiles(const std::vector<std::string>& filenames);

  /// Read the next data entry for a given category.
  ///
  /// @param name The category name for which to read the next entry
  /// @param collsToRead (optional) the collection names that should be available
  ///             for lazy reading. If not provided (or empty) all collections
  ///             will be available.
  ///
  /// @returns FrameData from which a podio::Frame can be constructed if the
  ///          category exists and if there are still entries left to read.
  ///          Otherwise a nullptr. No collection data is read at this point.
  ///
  /// @throws std::invalid_argument in case collsToRead contains collection
  /// names that are not available
  std::unique_ptr<podio::ROOTLazyFrameData> readNextEntry(std::string_view name,
                                                          const std::vector<std::string>& collsToRead = {});

  /// Read the desired data entry for a given category.
  ///
  /// @param name  The category name for which to read the next entry
  /// @param entry The entry number to read
  /// @param collsToRead (optional) the collection names that should be available
  ///              for lazy reading. If not provided (or empty) all collections
  ///              will be available.
  ///
  /// @returns FrameData from which a podio::Frame can be constructed if the
  ///          category and the desired entry exist. Otherwise a nullptr. No
  ///          collection data is read at this point.
  ///
  /// @throws std::invalid_argument in case collsToRead contains collection
  /// names that are not available
  std::unique_ptr<podio::ROOTLazyFrameData> readEntry(std::string_view name, const unsigned entry,
                                                      const std::vector<std::string>& collsToRead = {});

  /// Get the number of entries for the given name
  ///
  /// @param name The name of the category
  ///
  /// @returns The number of entries that are available for the category
  unsigned getEntries(std::string_view name) const;

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
  std::optional<podio::version::Version> currentFileVersion(std::string_view name) const {
    return m_datamodelHolder.getDatamodelVersion(name);
  }

  /// Get the names of all the available Frame categories in the current file(s).
  ///
  /// @returns The names of the available categories from the file
  std::vector<std::string_view> getAvailableCategories() const;

  /// Get the datamodel definition for the given name
  ///
  /// @param name The name of the datamodel
  ///
  /// @returns The high level definition of the datamodel in JSON format
  const std::string_view getDatamodelDefinition(std::string_view name) const {
    return m_datamodelHolder.getDatamodelDefinition(name);
  }

  /// Get all names of the datamodels that are available from this reader
  ///
  /// @returns The names of the datamodels
  std::vector<std::string> getAvailableDatamodels() const {
    return m_datamodelHolder.getAvailableDatamodels();
  }

private:
  /// Initialize the passed CategoryState by setting up the necessary branches,
  /// collection infos and all necessary meta data
  void initCategory(CategoryState& catState, std::string_view name);

  /// Get the category state for the given name. Initializes on first access.
  std::shared_ptr<CategoryState>& getCategoryState(std::string_view name);

  /// Read the parameters for a given entry
  GenericParameters readEntryParameters(CategoryState& catState, unsigned int localEntry);

  /// Read the data entry specified by the current entry counter in the given
  /// category state. Returns nullptr if out of bounds.
  std::unique_ptr<podio::ROOTLazyFrameData> readEntry(std::shared_ptr<CategoryState>& catState, unsigned entry,
                                                      const std::vector<std::string>& collsToRead);

  std::unique_ptr<TChain> m_metaChain{nullptr};
  std::unordered_map<std::string_view, std::shared_ptr<CategoryState>> m_categoryStates{};
  std::vector<std::string> m_availCategories{};

  podio::version::Version m_fileVersion{0, 0, 0};
  DatamodelDefinitionHolder m_datamodelHolder{};
};

} // namespace podio

#endif // PODIO_ROOTLAZYREADER_H
