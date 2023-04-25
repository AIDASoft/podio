#ifndef PODIO_COLLECTIONBRANCHES_H
#define PODIO_COLLECTIONBRANCHES_H

#include "TBranch.h"

#include <string>
#include <vector>

namespace podio::root_utils {
/**
 * Small helper struct to collect all branches that are necessary to read or
 * write a collection. Needed to cache the branch pointers and avoid having to
 * get them from a TTree/TChain for every event.
 */
struct CollectionBranches {
  TBranch* data{nullptr};
  std::vector<TBranch*> refs{};
  std::vector<TBranch*> vecs{};
  std::vector<std::string> refNames{}; ///< The names of the relation branches
  std::vector<std::string> vecNames{}; ///< The names of the vector member branches
};

} // namespace podio::root_utils

#endif
