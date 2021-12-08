#ifndef PODIO_ROOT_UTILS_H
#define PODIO_ROOT_UTILS_H

#include "podio/CollectionBase.h"
#include "podio/CollectionBranches.h"
#include "podio/CollectionIDTable.h"

#include "TBranch.h"
#include "TClass.h"

#include <iostream>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

namespace podio::root_utils {
// Workaround slow branch retrieval for 6.22/06 performance degradation
// see: https://root-forum.cern.ch/t/serious-degradation-of-i-o-performance-from-6-20-04-to-6-22-06/43584/10
template <class Tree>
TBranch* getBranch(Tree* chain, const char* name) {
  return static_cast<TBranch*>(chain->GetListOfBranches()->FindObject(name));
}

inline std::string refBranch(const std::string& name, size_t index) {
  return name + "#" + std::to_string(index);
}

inline std::string vecBranch(const std::string& name, size_t index) {
  return name + "_" + std::to_string(index);
}

inline void setCollectionAddresses(podio::CollectionBase* collection, const CollectionBranches& branches) {
  const auto collBuffers = collection->getBuffers();

  if (auto buffer = collBuffers.data) {
    branches.data->SetAddress(buffer);
  }

  if (auto refCollections = collBuffers.references) {
    for (size_t i = 0; i < refCollections->size(); ++i) {
      branches.refs[i]->SetAddress(&(*refCollections)[i]);
    }
  }

  if (auto vecMembers = collBuffers.vectorMembers) {
    for (size_t i = 0; i < vecMembers->size(); ++i) {
      branches.vecs[i]->SetAddress((*vecMembers)[i].second);
    }
  }
}

// A collection of additional information that describes the collection: the
// collectionID, the collection (data) type, and whether it is a subset
// collection
using CollectionInfoT = std::tuple<int, std::string, bool>;

/**
 * reconstruct the collection info from information that is available from other
 * trees in the file.
 *
 * NOTE: This function is only supposed to be called if there is no
 * "CollectionTypeInfo" branch in the metadata tree, as it assumes that the file
 * has been written with podio previous to #197 where there were no subset
 * collections
 */
inline auto reconstructCollectionInfo(TTree* eventTree, podio::CollectionIDTable const& idTable) {
  std::vector<CollectionInfoT> collInfo;

  for (size_t iColl = 0; iColl < idTable.names().size(); ++iColl) {
    const auto collID = idTable.ids()[iColl];
    const auto& name = idTable.names()[iColl];

    if (auto branch = getBranch(eventTree, name.c_str())) {
      const std::string_view bufferClassName = branch->GetClassName();
      // this comes with vector<...Data>, where we only care about the ...
      std::string_view dataClass = bufferClassName;
      dataClass.remove_suffix(5);
      const auto collClass = std::string(dataClass.substr(7)) + "Collection";
      // Assume that there are no subset collections in "old files"
      collInfo.emplace_back(collID, std::move(collClass), false);
    } else {
      std::cerr << "Problems reconstructing collection info for collection: \'" << name << "\'\n";
    }
  }

  return collInfo;
}

} // namespace podio::root_utils

#endif
