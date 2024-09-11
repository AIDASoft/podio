#ifndef PODIO_DETAIL_INTERFACEIOHELPERS_H
#define PODIO_DETAIL_INTERFACEIOHELPERS_H

#include "podio/utilities/TypeHelpers.h"
#include <podio/CollectionBase.h>

#include <tuple>
#include <vector>

namespace podio::detail {

/// Function template for handling interface types in OneToMultiRelations
///
/// Effectively this function checks whether the passed collection can be
/// dynamically cast to the collection type of the concrete type and if that is
/// true uses it to construct an interface type and add it to relElements. The
/// function on its own doesn't do anything too meaningful, it is meant to be
/// used in a call to std::apply that goes over all the interfaced types of an
/// interface type.
///
/// @tparam T The concrete type inside the interface that should be checked.
///           This effectively is mainly used for tag-dispatch and overload
///           selection in this context
/// @tparam InterfaceType The interface type (that can be used to interface T)
///
/// @param relElements The vector to which the interface objects should be added
/// @param coll The collection that holds the actual element
/// @param id The ObjectID of the element that we are currently looking for
template <typename T, typename InterfaceType>
void tryAddTo(T, std::vector<InterfaceType>& relElements, const podio::CollectionBase* coll, const podio::ObjectID id) {
  if (auto typedColl = dynamic_cast<const T::collection_type*>(coll)) {
    const T tmp = (*typedColl)[id.index];
    relElements.emplace_back(tmp);
  }
}

/// Helper function for handling interface type relations in OneToManyRelations
///
/// This function tries all types that are interfaced by the InterfaceType and
/// adds the one that matches to the relations. The main work happens in
/// tryAddTo, this simply wraps everything in a std::apply over all
/// interfaced_types.
///
/// @tparam InterfaceType The interface type of the Relation
///
/// @param relElements The vector to which the interface objects should be added
/// @param coll The collection that holds the actual element
/// @param id The ObjectID of the element that we are currently looking for
template <typename InterfaceType>
void addInterfaceToMultiRelation(std::vector<InterfaceType>& relElements, const podio::CollectionBase* coll,
                                 const podio::ObjectID id) {
  std::apply([&relElements, &coll, &id](auto... t) { (tryAddTo(t, relElements, coll, id), ...); },
             typename InterfaceType::interfaced_types{});
}

/// Helper function for adding an object to the OneToManyRelations container
/// when reading back collections
///
/// This function does the necessary type casting of the passed collection to
/// retrieve the desired object and also takes care of adding the object to the
/// container that holds them for later usage. It handles relations to regular
/// types as well as interface types.
///
/// This functionality has been lifted from the jinja2 templates, where we now
/// only call it, because we need a template deduction context for making if
/// constexpr work as expected, such that we can dispatch to different
/// implementatoins depending on whether the relation is to an interface type or
/// to a regular type.
///
/// @note It is expected that the following pre-conditions are met:
///       - The passed collection is valid (i.e. not a nullptr)
///       - the collectionID of the passed collection is the same as the one in
///         the passed ObjectID
///       - The collection can by casted to the relation type or any of the
///         interfaced types of the relation
///
/// @tparam RelType The type of the OneToManyRelation
///
/// @param relElements The container that holds the objects for the relation and
///                    which will be used for adding an element from the passed
///                    collection
/// @param coll The collection from which the object will be obtained after the
///             necessary type casting
/// @param id The ObjectID of the object that should be retrieved and added.
template <typename RelType>
void addMultiRelation(std::vector<RelType>& relElements, const podio::CollectionBase* coll, const podio::ObjectID id) {
  if constexpr (podio::detail::isInterfaceType<RelType>) {
    addInterfaceToMultiRelation(relElements, coll, id);
  } else {
    const auto* typeColl = static_cast<const RelType::collection_type*>(coll);
    relElements.emplace_back((*typeColl)[id.index]);
  }
}

/// Function template for handling interface types in OneToOneRelations
///
/// Effectively this function checks whether the passed collection can be
/// dynamically cast to the collection type of the concrete type and if that is
/// true uses it to assign to the passed interface object The function on its
/// own doesn't do anything too meaningful, it is meant to be used in a call to
/// std::apply that goes over all the interfaced types of an interface type.
///
/// @tparam T The concrete type inside the interface that should be checked.
///           This effectively is mainly used for tag-dispatch and overload
///           selection in this context
/// @tparam InterfaceType The interface type (that can be used to interface T)
///
/// @param relation The object to which the interface object should be assigned
///                 to
/// @param coll The collection that holds the actual element
/// @param id The ObjectID of the element that we are currently looking for
template <typename T, typename InterfaceType>
void tryAssignTo(T, InterfaceType*& relation, const podio::CollectionBase* coll, const podio::ObjectID id) {
  if (const auto* typeColl = dynamic_cast<const T::collection_type*>(coll)) {
    relation = new InterfaceType((*typeColl)[id.index]);
  }
}

/// Helper function for handling interface type relations in OneToOneRelations
///
/// This function tries all types that are interfaced by the InterfaceType and
/// that assigns the one thta matches to the relation. The main work happens in
/// tryAssignTo, this simply wraps everything in a std::apply over all
/// interfaced_types.
///
/// @tparam InterfaceType The interface type of the Relation
///
/// @param relation The object to which the interface object should be assigned
///                 to
/// @param coll The collection that holds the actual element
/// @param id The ObjectID of the element that we are currently looking for
template <typename InterfaceType>
void addInterfaceToSingleRelation(InterfaceType*& relation, const podio::CollectionBase* coll,
                                  const podio::ObjectID id) {
  std::apply([&relation, &coll, &id](auto... t) { (tryAssignTo(t, relation, coll, id), ...); },
             typename InterfaceType::interfaced_types{});
}

/// Helper function for assigning the related object in a OneToOneRelation
///
/// This function does the necessary type casting of the passed collection to
/// retrieve the desired object and also takes care of assigning the object to
/// the passed pointer that hold it for later usage. It handles relations
/// to regular types as well as interface types.
///
/// This functionality has been lifted from the jinja2 templates, where we now
/// only call it, because we need a template deduction context for making if
/// constexpr work as expected, such that we can dispatch to different
/// implementatoins depending on whether the relation is to an interface type or
/// to a regular type.
///
/// @note It is expected that the following pre-conditions are met:
///       - The passed collection is valid (i.e. not a nullptr)
///       - the collectionID of the passed collection is the same as the one in
///         the passed ObjectID
///       - The collection can by casted to the relation type or any of the
///         interfaced types of the relation
///
/// @tparam RelType The type of the OneToManyRelation
///
/// @param relation The pointer to which we should assign the related object
///                 that is retrieved from the passed collection
/// @param coll The collection from which the object will be obtained after the
///             necessary type casting
/// @param id The ObjectID of the object that should be retrieved and added.
template <typename RelType>
void addSingleRelation(RelType*& relation, const podio::CollectionBase* coll, const podio::ObjectID id) {
  if constexpr (podio::detail::isInterfaceType<RelType>) {
    addInterfaceToSingleRelation(relation, coll, id);
  } else {
    const auto* typeColl = static_cast<const RelType::collection_type*>(coll);
    relation = new RelType((*typeColl)[id.index]);
  }
}

} // namespace podio::detail

#endif // PODIO_DETAIL_INTERFACEIOHELPERS_H
