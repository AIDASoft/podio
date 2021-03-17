#ifndef PODIO_COLLECTION_BRANCHES_H
#define PODIO_COLLECTION_BRANCHES_H

#include "TBranch.h"

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
};

} // namespace podio::root_utils

#endif
