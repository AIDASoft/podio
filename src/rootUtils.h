#ifndef PODIO_ROOT_UTILS_H // NOLINT(llvm-header-guard): internal headers confuse clang-tidy
#define PODIO_ROOT_UTILS_H // NOLINT(llvm-header-guard): internal headers confuse clang-tidy

#include "podio/CollectionBase.h"
#include "podio/CollectionBranches.h"
#include "podio/CollectionBuffers.h"
#include "podio/CollectionIDTable.h"

#include "TBranch.h"
#include "TChain.h"
#include "TClass.h"
#include "TTree.h"

#include <algorithm>
#include <cctype>
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
 * Names of the fields with the keys and values of the generic parameters for
 * the RNTuples until map types are supported
 */
constexpr static auto intKeyName = "GPIntKeys";
constexpr static auto floatKeyName = "GPFloatKeys";
constexpr static auto doubleKeyName = "GPDoubleKeys";
constexpr static auto stringKeyName = "GPStringKeys";

constexpr static auto intValueName = "GPIntValues";
constexpr static auto floatValueName = "GPFloatValues";
constexpr static auto doubleValueName = "GPDoubleValues";
constexpr static auto stringValueName = "GPStringValues";

/**
 * Get the name of the key depending on the type
 */
template <typename T>
constexpr auto getGPKeyName() {
  if constexpr (std::is_same<T, int>::value) {
    return intKeyName;
  } else if constexpr (std::is_same<T, float>::value) {
    return floatKeyName;
  } else if constexpr (std::is_same<T, double>::value) {
    return doubleKeyName;
  } else if constexpr (std::is_same<T, std::string>::value) {
    return stringKeyName;
  } else {
    static_assert(sizeof(T) == 0, "Unsupported type for generic parameters");
  }
}

/**
 * Get the name of the value depending on the type
 */
template <typename T>
constexpr auto getGPValueName() {
  if constexpr (std::is_same<T, int>::value) {
    return intValueName;
  } else if constexpr (std::is_same<T, float>::value) {
    return floatValueName;
  } else if constexpr (std::is_same<T, double>::value) {
    return doubleValueName;
  } else if constexpr (std::is_same<T, std::string>::value) {
    return stringValueName;
  } else {
    static_assert(sizeof(T) == 0, "Unsupported type for generic parameters");
  }
}

/**
 * Name of the field with the list of categories for RNTuples
 */
constexpr static auto availableCategories = "availableCategories";

/**
 * Name of the field with the names of the collections for RNTuples
 */
inline std::string collectionName(const std::string& category) {
  return category + "_collectionNames";
}

/**
 * Name of the field with the flag for subset collections for RNTuples
 */
inline std::string subsetCollection(const std::string& category) {
  return category + "_isSubsetCollections";
}

/**
 * The name of the branch into which we store the build version of podio at the
 * time of writing the file
 */
constexpr static auto versionBranchName = "PodioBuildVersion";

/**
 * The name of the branch in which all the EDM names and their definitions are
 * stored in the meta data tree.
 */
constexpr static auto edmDefBranchName = "EDMDefinitions";

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

inline std::string refBranch(const std::string& name, std::string_view relName) {
  return "_" + name + "_" + std::string(relName);
}

inline std::string vecBranch(const std::string& name, size_t index) {
  return name + "_" + std::to_string(index);
}

inline std::string vecBranch(const std::string& name, std::string_view vecName) {
  return "_" + name + "_" + std::string(vecName);
}

/// The name for subset branches
inline std::string subsetBranch(const std::string& name) {
  return name + "_objIdx";
}

/**
 * Reset all the branches that by getting them from the TTree again
 */
inline void resetBranches(TTree* chain, CollectionBranches& branches, const std::string& name) {
  if (branches.data) {
    branches.data = getBranch(chain, name);
  }

  for (size_t i = 0; i < branches.refs.size(); ++i) {
    branches.refs[i] = getBranch(chain, branches.refNames[i]);
  }

  for (size_t i = 0; i < branches.vecs.size(); ++i) {
    branches.vecs[i] = getBranch(chain, branches.vecNames[i]);
  }
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
using CollectionInfoT = std::tuple<uint32_t, std::string, bool, unsigned int>;
// for backwards compatibility
using CollectionInfoWithoutSchemaT = std::tuple<int, std::string, bool>;

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

/**
 * Sort the input vector of strings alphabetically, case insensitive.
 */
inline std::vector<std::string> sortAlphabeticaly(std::vector<std::string> strings) {
  // Obviously there is no tolower(std::string) in c++, so this is slightly more
  // involved and we make use of the fact that lexicographical_compare works on
  // ranges and the fact that we can feed it a dedicated comparison function,
  // where we convert the strings to lower case char-by-char. The alternative is
  // to make string copies inside the first lambda, transform them to lowercase
  // and then use operator< of std::string, which would be effectively
  // hand-writing what is happening below.
  std::sort(strings.begin(), strings.end(), [](const auto& lhs, const auto& rhs) {
    return std::lexicographical_compare(
        lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
        [](const auto& cl, const auto& cr) { return std::tolower(cl) < std::tolower(cr); });
  });
  return strings;
}

} // namespace podio::root_utils

#endif
