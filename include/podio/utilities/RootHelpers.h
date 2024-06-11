#ifndef PODIO_UTILITIES_ROOTHELPERS_H
#define PODIO_UTILITIES_ROOTHELPERS_H

#include "TBranch.h"

#include <string>
#include <tuple>
#include <vector>

namespace podio {
class CollectionBase;

namespace root_utils {

  // A collection of additional information that describes the collection: the
  // collectionID, the collection (data) type, whether it is a subset
  // collection, and its schema version
  using CollectionWriteInfoT = std::tuple<uint32_t, std::string, bool, unsigned int>;
  // for backwards compatibility
  using CollectionInfoWithoutSchemaT = std::tuple<int, std::string, bool>;

  /// A collection name and a base pointer grouped together for writing
  using StoreCollection = std::tuple<const std::string&, podio::CollectionBase*>;

  /// Small helper struct to collect all branches that are necessary to read or
  /// write a collection. Needed to cache the branch pointers and avoid having to
  /// get them from a TTree/TChain for every event.
  struct CollectionBranches {
    TBranch* data{nullptr};
    std::vector<TBranch*> refs{};
    std::vector<TBranch*> vecs{};
    std::vector<std::string> refNames{}; ///< The names of the relation branches
    std::vector<std::string> vecNames{}; ///< The names of the vector member branches
  };

  /// Pair of keys and values for one type of the ones that can be stored in
  /// GenericParameters
  template <typename T>
  using ParamStorage = std::tuple<std::vector<std::string>, std::vector<std::vector<T>>>;

} // namespace root_utils
} // namespace podio

#endif // PODIO_UTILITIES_ROOTHELPERS_H
