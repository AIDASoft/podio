#ifndef PODIO_RNTUPLEWRITER_H
#define PODIO_RNTUPLEWRITER_H

#include "podio/Frame.h"
#include "podio/GenericParameters.h"
#include "podio/SchemaEvolution.h"
#include "podio/utilities/DatamodelRegistryIOHelpers.h"
#include "podio/utilities/RootHelpers.h"

#include "TFile.h"
#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleModel.hxx>
#include <RVersion.h>
#if ROOT_VERSION_CODE >= ROOT_VERSION(6, 31, 0)
  #include <ROOT/RNTupleWriter.hxx>
#endif

#include <string>
#include <unordered_map>
#include <vector>

namespace podio {

/// The RNTupleWriter writes podio files into ROOT files using the new RNTuple
/// format.
///
/// Each category gets its own RNTuple. Additionally, there is a podio_metadata
/// RNTuple that contains metadata that is necessary for interpreting the files
/// for reading.
///
/// Files written with the RNTupleWriter can be read with the RNTupleReader.
class RNTupleWriter {
public:
  /// Create a RNTupleWriter to write to a file.
  ///
  /// @note Existing files will be overwritten without warning.
  ///
  /// @param filename The path to the file that will be created.
  RNTupleWriter(const std::string& filename);

  /// RNTupleWriter destructor
  ///
  /// This also takes care of writing all the necessary metadata in order to be
  /// able to read files back again.
  ~RNTupleWriter();

  /// The RNTupleWriter is not copy-able
  RNTupleWriter(const RNTupleWriter&) = delete;
  /// The RNTupleWriter is not copy-able
  RNTupleWriter& operator=(const RNTupleWriter&) = delete;

  /// Store the given frame with the given category.
  ///
  /// This stores all available collections from the Frame.
  ///
  /// @note The contents of the first Frame that is written in this way
  /// determines the contents that will be written for all subsequent Frames.
  ///
  /// @param frame    The Frame to store
  /// @param category The category name under which this Frame should be stored
  void writeFrame(const podio::Frame& frame, const std::string& category);

  /// Store the given Frame with the given category.
  ///
  /// This stores only the desired collections and not the complete frame.
  ///
  /// @note The contents of the first Frame that is written in this way
  /// determines the contents that will be written for all subsequent Frames.
  ///
  /// @param frame        The Frame to store
  /// @param category     The category name under which this Frame should be
  ///                     stored
  /// @param collsToWrite The collection names that should be written
  void writeFrame(const podio::Frame& frame, const std::string& category, const std::vector<std::string>& collsToWrite);

  /// Write the current file, including all the necessary metadata to read it
  /// again.
  ///
  /// @note The destructor will also call this, so letting a RNTupleWriter go out
  /// of scope is also a viable way to write a readable file
  void finish();

  /// Check whether the collsToWrite are consistent with the state of the passed
  /// category.
  ///
  /// @note This will only be a meaningful check if the first Frame of the passed
  /// category has already been written. Also, this check is rather expensive as
  /// it has to effectively do two set differences.
  ///
  ///
  /// @param collsToWrite The collection names that should be checked for
  ///                     consistency
  /// @param category     The category name for which consistency should be
  ///                     checked
  ///
  /// @returns two vectors of collection names. The first one contains all the
  /// names that were missing from the collsToWrite but were present in the
  /// category. The second one contains the names that are present in the
  /// collsToWrite only. If both vectors are empty the category and the passed
  /// collsToWrite are consistent.
  std::tuple<std::vector<std::string>, std::vector<std::string>>
  checkConsistency(const std::vector<std::string>& collsToWrite, const std::string& category) const;

private:
  template <typename T>
  void fillParams(GenericParameters& params, ROOT::Experimental::REntry* entry);

  std::unique_ptr<ROOT::Experimental::RNTupleModel>
  createModels(const std::vector<root_utils::StoreCollection>& collections);

  std::unique_ptr<ROOT::Experimental::RNTupleModel> m_metadata{};
  std::unique_ptr<ROOT::Experimental::RNTupleWriter> m_metadataWriter{};

  std::unique_ptr<TFile> m_file{};

  DatamodelDefinitionCollector m_datamodelCollector{};

  struct CollectionInfo {
    std::vector<uint32_t> id{};
    std::vector<std::string> name{};
    std::vector<std::string> type{};
    std::vector<short> isSubsetCollection{};
    std::vector<SchemaVersionT> schemaVersion{};
    std::unique_ptr<ROOT::Experimental::RNTupleWriter> writer{nullptr};
  };
  CollectionInfo& getCategoryInfo(const std::string& category);

  std::unordered_map<std::string, CollectionInfo> m_categories{};

  bool m_finished{false};

  std::vector<std::string> m_intkeys{}, m_floatkeys{}, m_doublekeys{}, m_stringkeys{};

  std::vector<std::vector<int>> m_intvalues{};
  std::vector<std::vector<float>> m_floatvalues{};
  std::vector<std::vector<double>> m_doublevalues{};
  std::vector<std::vector<std::string>> m_stringvalues{};

  template <typename T>
  std::pair<std::vector<std::string>&, std::vector<std::vector<T>>&> getKeyValueVectors();
};

} // namespace podio

#endif // PODIO_RNTUPLEWRITER_H
