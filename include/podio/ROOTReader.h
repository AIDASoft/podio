#ifndef PODIO_ROOTREADER_H
#define PODIO_ROOTREADER_H

#include "podio/ROOTFrameData.h"
#include "podio/utilities/ReaderCommon.h"
#include "podio/utilities/ReaderUtils.h"
#include "podio/utilities/RootHelpers.h"

#include "TChain.h"

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// forward declarations
class TClass;
class TFile;
class TTree;

namespace podio {

class CollectionBase;
class CollectionIDTable;
class GenericParameters;
struct CollectionReadBuffers;

/// This class has the function to read available data from disk in ROOTs TTree
/// format.
///
/// The ROOTReader provides the data as ROOTFrameData from which a podio::Frame
/// can be constructed. It can be used to read files written by the ROOTWriter.
class ROOTReader : public ReaderCommon, root_utils::TTreeReaderCommon {

public:
  ROOTReader() = default;
  ~ROOTReader() = default;

  ROOTReader(const ROOTReader&) = delete;
  ROOTReader& operator=(const ROOTReader&) = delete;
  ROOTReader(ROOTReader&&) = default;
  ROOTReader& operator=(ROOTReader&&) = default;

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
  std::unique_ptr<podio::ROOTFrameData> readNextEntry(std::string_view name,
                                                      const std::vector<std::string>& collsToRead = {});

  /// Read the desired data entry for a given category.
  ///
  /// @param name  The category name for which to read the next entry
  /// @param entry The entry number to read
  /// @param collsToRead (optional) the collection names that should be read. If
  ///              not provided (or empty) all collections will be read
  ///
  /// @returns FrameData from which a podio::Frame can be constructed if the
  ///          category and the desired entry exist. Otherwise a nullptr
  ///
  /// @throws std::invalid_argument in case collsToRead contains collection
  /// names that are not available
  std::unique_ptr<podio::ROOTFrameData> readEntry(std::string_view name, const unsigned entry,
                                                  const std::vector<std::string>& collsToRead = {});

  /// Get the number of entries for the given name
  ///
  /// @param name The name of the category
  ///
  /// @returns The number of entries that are available for the category
  unsigned getEntries(std::string_view name) const;

  /// Get the names of all the available Frame categories in the current file(s).
  ///
  /// @returns The names of the available categories from the file
  std::vector<std::string_view> getAvailableCategories() const;

  std::optional<std::map<std::string, SizeStats>> getSizeStats(std::string_view category);

private:
  /// Helper struct to group together all the necessary state to read / process
  /// a given category. A "category" in this case describes all frames with the
  /// same name which are constrained by the ROOT file structure that we use to
  /// have the same contents. It encapsulates all state that is necessary for
  /// reading from a TTree / TChain (i.e. collection infos, branches, ...)
  struct CategoryInfo {
    /// constructor from chain for more convenient map insertion
    CategoryInfo(std::unique_ptr<TChain>&& c) : chain(std::move(c)) {
    }
    std::unique_ptr<TChain> chain{nullptr};                      ///< The TChain with the data
    unsigned entry{0};                                           ///< The next entry to read
    std::vector<NamedCollInfo> storedClasses{};                  ///< The stored collections in this
                                                                 ///< category
    std::vector<root_utils::CollectionBranches> branches{};      ///< The (data) branches for this category
    std::vector<root_utils::CollectionBranches> paramBranches{}; ///< The parameter branches for this category
    std::shared_ptr<CollectionIDTable> table{nullptr};           ///< The collection ID table for this category
  };

  /// Initialize the passed CategoryInfo by setting up the necessary branches,
  /// collection infos and all necessary meta data to be able to read entries
  /// with this name
  void initCategory(CategoryInfo& catInfo, std::string_view name);

  /// Get the category information for the given name. In case there is no TTree
  /// with contents for the given name this will return a CategoryInfo with an
  /// uninitialized chain (nullptr) member
  CategoryInfo& getCategoryInfo(std::string_view name);

  /// Read the parameters for the entry specified in the passed CategoryInfo
  GenericParameters readEntryParameters(CategoryInfo& catInfo, bool reloadBranches, unsigned int localEntry);

  /// Read the data entry specified in the passed CategoryInfo, and increase the
  /// counter afterwards. In case the requested entry is larger than the
  /// available number of entries, return a nullptr.
  std::unique_ptr<podio::ROOTFrameData> readEntry(ROOTReader::CategoryInfo& catInfo,
                                                  const std::vector<std::string>& collsToRead);

  /// Get / read the buffers at index iColl in the passed category information
  std::optional<podio::CollectionReadBuffers> getCollectionBuffers(CategoryInfo& catInfo, size_t iColl,
                                                                   bool reloadBranches, unsigned int localEntry);

  std::unique_ptr<TChain> m_metaChain{nullptr};                      ///< The metadata tree
  std::unordered_map<std::string_view, CategoryInfo> m_categories{}; ///< All categories
  std::vector<std::string> m_availCategories{};                      ///< All available categories from this file
};

} // namespace podio

#endif // PODIO_ROOTREADER_H
