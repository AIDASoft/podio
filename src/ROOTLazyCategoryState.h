#ifndef PODIO_ROOTLAZYCATEGORYSTATE_H
#define PODIO_ROOTLAZYCATEGORYSTATE_H

#include "podio/CollectionIDTable.h"
#include "podio/ROOTReader.h"
#include "podio/utilities/RootHelpers.h"

#include "TChain.h"

#include <memory>
#include <mutex>
#include <vector>

namespace podio {

/// Shared state between ROOTLazyReader and ROOTLazyFrameData instances for the
/// same category. Holds the TChain and branch information needed for lazy I/O.
/// The mutex serializes all ROOT I/O operations since ROOT is not thread-safe.
struct CategoryState {
  CategoryState() = default;
  ~CategoryState() = default;
  CategoryState(const CategoryState&) = delete;
  CategoryState& operator=(const CategoryState&) = delete;
  CategoryState(CategoryState&&) = delete;
  CategoryState& operator=(CategoryState&&) = delete;

  std::unique_ptr<TChain> chain{nullptr};
  unsigned entry{0};
  std::vector<detail::NamedCollInfo> storedClasses{};
  std::vector<root_utils::CollectionBranches> branches{};
  std::shared_ptr<const podio::CollectionIDTable> table{nullptr};
  std::vector<root_utils::CollectionBranches> paramBranches{};
  std::mutex mutex{};
};

} // namespace podio

#endif // PODIO_ROOTLAZYCATEGORYSTATE_H
