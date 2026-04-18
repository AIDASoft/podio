#ifndef PODIO_UTILITIES_RNTUPLEHELPERS_H
#define PODIO_UTILITIES_RNTUPLEHELPERS_H

#include "podio/CollectionIDTable.h"
#include "podio/GenericParameters.h"
#include "podio/podioVersion.h"
#include "podio/utilities/DatamodelRegistryIOHelpers.h"
#include "podio/utilities/RootHelpers.h"

#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleReader.hxx>
#include <RVersion.h>

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

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

/// Base struct that factors out the common metadata handling shared between
/// RNTupleReader and RNTupleLazyReader. Mirrors the role that
/// root_utils::TTreeReaderCommon plays for the TTree-based readers.
struct RNTupleReaderCommon {
public:
  /// Get the names of all the available Frame categories in the current file(s).
  std::vector<std::string_view> getAvailableCategories() const;

protected:
  /// Open per-file metadata readers, read the podio version, EDM definitions
  /// and the list of available categories. Writes into fileVersion and
  /// datamodelHolder (which live in the ReaderCommon base class).
  void openMetaData(const std::vector<std::string>& filenames, podio::version::Version& fileVersion,
                    podio::DatamodelDefinitionHolder& datamodelHolder);

  /// Read and reconstruct the generic parameters of the Frame from the given
  /// metadata reader at the given local entry index.
  GenericParameters readEventMetaData(root_compat::RNTupleReader* reader, unsigned localEntry);

  /// Read the CollectionWriteInfo for the given category from the first file's
  /// metadata reader and build the CollectionIDTable. Both output parameters are
  /// written only on success (return value true). Returns false if the category
  /// is not in m_availableCategories.
  bool initCategory(std::string_view category, std::vector<podio::root_utils::CollectionWriteInfo>& collInfo,
                    std::shared_ptr<const podio::CollectionIDTable>& idTable);

  std::unique_ptr<root_compat::RNTupleReader> m_metadata{};
  std::unordered_map<std::string, std::unique_ptr<root_compat::RNTupleReader>> m_metadata_readers{};
  std::vector<std::string> m_filenames{};
  std::vector<std::string> m_availableCategories{};
};

} // namespace podio

#endif // PODIO_UTILITIES_RNTUPLEHELPERS_H
