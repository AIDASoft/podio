#ifndef PODIO_FRAMEPOLICIES_H
#define PODIO_FRAMEPOLICIES_H

#include "podio/utilities/TypeHelpers.h"

#include <stdexcept>
#include <string>
#include <type_traits>

namespace podio {
template <typename Policy, typename CollT>
concept GetPolicyCallable = CollectionType<CollT> && requires(Policy policy, CollT coll) {
  {
    policy.template operator()<CollT>(std::declval<std::add_pointer_t<std::add_const_t<CollT>>>(),
                                      std::declval<std::add_const_t<std::add_lvalue_reference_t<std::string>>>())
  } -> std::same_as<std::add_lvalue_reference_t<std::add_const_t<CollT>>>;
};

struct FrameCreateEmptyNonExistentPolicy {
  template <podio::CollectionType CollT>
  const CollT& operator()(const CollT* coll, const std::string&) const {
    if (coll) {
      return *coll;
    }

    static const auto emptyColl = CollT();
    return emptyColl;
  }
};

struct FrameThrowOnNonExistentPolicy {
  template <podio::CollectionType CollT>
  const CollT& operator()(const CollT* coll, const std::string& name) const {
    if (coll) {
      return *coll;
    }
    throw std::runtime_error("Cannot retrieve collection " + name + " from Frame");
  }
};
} // namespace podio

#endif // PODIO_FRAMEPOLICIES_H
