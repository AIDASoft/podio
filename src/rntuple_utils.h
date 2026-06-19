#ifndef PODIO_RNTUPLE_UTILS_H // NOLINT(llvm-header-guard): internal headers confuse clang-tidy
#define PODIO_RNTUPLE_UTILS_H // NOLINT(llvm-header-guard): internal headers confuse clang-tidy

#include "podio/CollectionBuffers.h"
#include "podio/DatamodelRegistry.h"
#include "podio/GenericParameters.h"
#include "podio/utilities/RNTupleHelpers.h"
#include "rootUtils.h"

#include <ROOT/RError.hxx>
#include <RVersion.h>

#include <memory>
#include <vector>

// Adjust for the move of this out of ROOT v7 in
// https://github.com/root-project/root/pull/17281
#if ROOT_VERSION_CODE >= ROOT_VERSION(6, 35, 0)
using ROOT::RException;
#else
using ROOT::Experimental::RException;
#endif

namespace podio::rntuple_utils {

/// Read the generic parameters of one type from a metadata reader entry and
/// store them in params.
template <typename T>
void readParams(root_compat::RNTupleReader* reader, const unsigned localEntry, GenericParameters& params) {
  auto keyView = reader->GetView<std::vector<std::string>>(root_utils::getGPKeyName<T>());
  auto valueView = reader->GetView<std::vector<std::vector<T>>>(root_utils::getGPValueName<T>());
  params.loadFrom(keyView(localEntry), valueView(localEntry));
}

/// Bind all fields of the given collection to dentry and populate collBuffers.
/// Returns false if a ROOT field is missing (RException was thrown), in which
/// case collBuffers.deleteBuffers has been cleared to prevent a double-free on
/// partial binding.
template <typename EntryT>
bool bindCollectionToEntry(EntryT* dentry, podio::CollectionReadBuffers& collBuffers,
                           const podio::root_utils::CollectionWriteInfo& coll) {
  const auto& collType = coll.dataType;
  try {
    if (coll.isSubset) {
      const auto brName = root_utils::subsetBranch(coll.name);
      const auto vec = new std::vector<podio::ObjectID>;
      dentry->BindRawPtr(brName, vec);
      collBuffers.references->at(0) = std::unique_ptr<std::vector<podio::ObjectID>>(vec);
    } else {
      dentry->BindRawPtr(coll.name, collBuffers.data);

      const auto relVecNames = podio::DatamodelRegistry::instance().getRelationNames(collType);
      for (size_t j = 0; j < relVecNames.relations.size(); ++j) {
        const auto relName = relVecNames.relations[j];
        const auto vec = new std::vector<podio::ObjectID>;
        const auto brName = root_utils::refBranch(coll.name, relName);
        dentry->BindRawPtr(brName, vec);
        collBuffers.references->at(j) = std::unique_ptr<std::vector<podio::ObjectID>>(vec);
      }

      for (size_t j = 0; j < relVecNames.vectorMembers.size(); ++j) {
        const auto vecName = relVecNames.vectorMembers[j];
        const auto brName = root_utils::vecBranch(coll.name, vecName);
        dentry->BindRawPtr(brName, collBuffers.vectorMembers->at(j).second);
      }
    }
  } catch (const RException&) {
    collBuffers.deleteBuffers = {};
    return false;
  }
  return true;
}

} // namespace podio::rntuple_utils

#endif // PODIO_RNTUPLE_UTILS_H
