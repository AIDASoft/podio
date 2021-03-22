#ifndef PODIO_ROOT_UTILS_H__
#define PODIO_ROOT_UTILS_H__

#include "podio/CollectionBase.h"

#include "TBranch.h"
#include "TChain.h"
#include "TClass.h"

#include <vector>
#include <string>

namespace podio::root_utils {
// test workaround function for 6.22/06 performance degradation
// see: https://root-forum.cern.ch/t/serious-degradation-of-i-o-performance-from-6-20-04-to-6-22-06/43584/10
template<class Tree>
TBranch* getBranch(Tree* chain, const char* name) {
  return static_cast<TBranch*>(chain->GetListOfBranches()->FindObject(name));
}

struct CollectionBranches {
  TBranch* data{nullptr};
  std::vector<TBranch*> refs{};
  std::vector<TBranch*> vecs{};
};

inline std::string refBranch(const std::string& name, size_t index) {
  return name + "#" + std::to_string(index);
}

inline std::string vecBranch(const std::string& name, size_t index) {
  return name + "_" + std::to_string(index);
}


inline void setCollectionAddresses(podio::CollectionBase* collection, const CollectionBranches& branches) {
  auto refCollections = collection->referenceCollections();
  auto vecMembers = collection->vectorMembers();

  branches.data->SetAddress(collection->getBufferAddress());

  if (refCollections) {
    for (size_t i = 0; i < refCollections->size(); ++i) {
      branches.refs[i]->SetAddress(&(*refCollections)[i]);
    }
  }

  if (vecMembers) {
    for (size_t i = 0; i < vecMembers->size(); ++i) {
      branches.vecs[i]->SetAddress((*vecMembers)[i].second);
    }
  }
}

inline CollectionBase* prepareCollection(const TClass* dataClass, const TClass* collectionClass) {
  auto* buffer = dataClass->New();
  auto* collection = static_cast<CollectionBase*>(collectionClass->New());
  collection->setBuffer(buffer);
  return collection;
}


}

#endif
