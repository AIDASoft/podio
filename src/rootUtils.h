#ifndef PODIO_ROOT_UTILS_H // NOLINT(llvm-header-guard): internal headers confuse clang-tidy
#define PODIO_ROOT_UTILS_H // NOLINT(llvm-header-guard): internal headers confuse clang-tidy

#include "podio/CollectionBase.h"
#include "podio/CollectionBranches.h"
#include "podio/CollectionBuffers.h"
#include "podio/CollectionIDTable.h"

#include "TBranch.h"
#include "TClass.h"
#include "TTree.h"

#include <iostream>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

namespace podio::root_utils {
/**
 * The name of the meta data tree in podio ROOT files. This tree mainly stores
 * meta data that is necessary for ROOT based I/O.
 */
constexpr static auto metaTreeName = "podio_metadata";

/**
 * The name of the branch in the TTree for each frame for storing the
 * GenericParameters
 */
constexpr static auto paramBranchName = "PARAMETERS";

/**
 * The name of the branch into which we store the build version of podio at the
 * time of writing the file
 */
constexpr static auto versionBranchName = "PodioBuildVersion";

/**
 * Name of the branch for storing the idTable for a given category in the meta
 * data tree
 */
inline std::string idTableName(const std::string& category) {
  constexpr static auto suffix = "___idTable";
  return category + suffix;
}

/**
 * Name of the branch for storing the collection info for a given category in
 * the meta data tree
 */
inline std::string collInfoName(const std::string& category) {
  constexpr static auto suffix = "___CollectionTypeInfo";
  return category + suffix;
}

// Workaround slow branch retrieval for 6.22/06 performance degradation
// see: https://root-forum.cern.ch/t/serious-degradation-of-i-o-performance-from-6-20-04-to-6-22-06/43584/10
template <class Tree>
TBranch* getBranch(Tree* chain, const char* name) {
  return static_cast<TBranch*>(chain->GetListOfBranches()->FindObject(name));
}

template <typename Tree>
TBranch* getBranch(Tree* chain, const std::string& name) {
  return getBranch(chain, name.c_str());
}

inline std::string refBranch(const std::string& name, size_t index) {
  return name + "#" + std::to_string(index);
}

inline std::string vecBranch(const std::string& name, size_t index) {
  return name + "_" + std::to_string(index);
}

template <typename BufferT>
inline void setCollectionAddresses(const BufferT& collBuffers, const CollectionBranches& branches) {

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
// collectionID, the collection (data) type, whether it is a subset
// collection, and its schema version
using CollectionInfoT = std::tuple<int, std::string, bool, unsigned int>;
// for backwards compatibility
using CollectionInfoTWithoutSchema = std::tuple<int, std::string, bool>;

inline void readBranchesData(const CollectionBranches& branches, Long64_t entry) {
  // Read all data
  if (branches.data) {
    branches.data->GetEntry(entry);
  }
  for (auto* br : branches.refs) {
    br->GetEntry(entry);
  }
  for (auto* br : branches.vecs) {
    br->GetEntry(entry);
  }
}

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
      // Assume that there are no subset collections in "old files" and schema is 0
      collInfo.emplace_back(collID, std::move(collClass), false, 0);
    } else {
      std::cerr << "Problems reconstructing collection info for collection: \'" << name << "\'\n";
    }
  }

  return collInfo;
}

} // namespace podio::root_utils

#endif
