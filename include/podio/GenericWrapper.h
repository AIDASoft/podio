#ifndef PODIO_GENERICWRAPPER_H
#define PODIO_GENERICWRAPPER_H

#include "podio/ObjectID.h"

#include <type_traits>
#include <variant>
#include <iostream>

namespace podio {
namespace detail {

#if __cplusplus > 201704L
// Only available in c++20
using std::remove_cvref_t;
#else
// Simple enough to roll our own quickly
template<class T>
struct remove_cvref {
  using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template<class T>
using remove_cvref_t = typename remove_cvref<T>::type;
#endif

// Helper bool to check if the first type is any of the passed other types after
// stripping all const, volatile and references from the first type
template<typename T, typename ...Ts>
constexpr bool isAnyOf = (std::is_same_v<remove_cvref_t<T>, Ts> || ...);
// I.e. the following works
static_assert(isAnyOf<const int&, int>);

// Helper struct to select functions/overloads depending on whether the first
// type is actually one of the other passed types (after removing any const and
// volatile qualifiers). See also isAnyOf
template<typename T, typename ...Ts>
using EnableIfAnyOf = std::enable_if_t<isAnyOf<T, Ts...>>;

// Helper type to select functions/overloads depending on whether the condition
// C is treu and the type T is one of the other passed types (after removing any
// const and volatile qualifiers from T). See also isAnyOf
template <bool C, typename T, typename... Ts>
using EnableIfAnyOfAnd = std::enable_if_t<C && isAnyOf<T, Ts...>>;

// Helper struct to determine whether a type as an ObjPtrT subtype
// Mainly used for slightly nicer error messages
template<typename T, typename=std::void_t<>>
struct HasObjPtr : std::false_type {};

template<typename T>
struct HasObjPtr<T, std::void_t<typename T::ObjPtrT>> : std::true_type {};

template<typename T>
constexpr bool hasObjPtr = HasObjPtr<T>::value;

template<typename ...Ts>
constexpr bool allHaveObjPtrT = (hasObjPtr<Ts> && ...);

// Helper type for getting the type of the Obj pointer that is stored in the
// class.
// NOTE: Not SFINAE friendly by design!
template<typename T>
struct GetObjPtr {
  using type = typename T::ObjPtrT;
};

template<typename T>
using GetObjPtrT = typename GetObjPtr<T>::type;

// Helper type for getting the Const type from a mutable class (which is used to
// instantiate the templates)
// NOTE: no checks here, since we assume that that is covered by the checks for
// the public ObjPtrT subtype above
template<typename T>
struct GetConstType {
  using type = typename T::ConstT;
};

template<typename T>
using GetConstT = typename GetConstType<T>::type;

} // namespace detail


/**
 * GenericWrapper class that can wrap any number of DataTypes as long as they
 * have a public ObjPtrT typedef/subtype. The wrapper internally stores the Obj*
 * of the passed value type into a std::variant of all the passed types. It also
 * defines some commonly used functions that all data types provide. The
 * intended use case is to inherit form this GenericWrapper and then implement
 * necessary additional functionality in the inheriting class that can then be
 * used just as any other podio generated class.
 *
 * NOTE: The first bool template parameter steers whether a call to getValue can
 * return only Const values or if it can also return mutable values.
 */
template<bool Mutable, typename ...WrappedTypes>
class GenericWrapper {
  static_assert(detail::allHaveObjPtrT<WrappedTypes...>, "All WrappedTypes must have a public ObjPtrT subtype");
  /// Private type of the variant that is used internally
  using VariantT = typename std::variant<detail::GetObjPtrT<WrappedTypes>...>;

  /// Private helper type for enabling functions that should work with "Const"
  /// and the default classes
  template<typename T>
  using EnableIfValueType =  detail::EnableIfAnyOf<T,
                                                   WrappedTypes...,
                                                   detail::GetConstT<WrappedTypes>...>;

  /// Private helper type for enabling functions that whould work for ObjPtrT
  /// template arguments
  template<typename T>
  using EnableIfObjPtrType = detail::EnableIfAnyOf<T,
                                                   detail::GetObjPtrT<WrappedTypes>...>;


public:
  /// Public helper type for enabling one "default" constructor in the using
  /// classes that takes values or Obj pointers
  template<typename T>
  using EnableWrapper = detail::EnableIfAnyOf<T,
                                              WrappedTypes...,
                                              detail::GetConstT<WrappedTypes>...,
                                              detail::GetObjPtrT<WrappedTypes>...
                                              >;

  template<typename T,
           typename = EnableIfValueType<T>>
  GenericWrapper(T value) : m_obj(value.m_obj) {
    value.m_obj->acquire(); // TODO: go through std::visit as well here?
  }

  template<typename ObjT,
           typename = EnableIfObjPtrType<ObjT>>
  GenericWrapper(ObjT* obj) : m_obj(obj) {
    obj->acquire(); // TODO: go through std::visit as well here?
  }

  ~GenericWrapper() {
    releaseObj();
  }

  // No default constructor as the "empty" wrapper is really not something that
  // makes sense in this case
  GenericWrapper() = delete;

  GenericWrapper(GenericWrapper const& other) : m_obj(other.m_obj) {
    acquireObj();
  }

  GenericWrapper(VariantT const& variant) : m_obj(variant) {
    acquireObj();
  }

  GenericWrapper& operator=(GenericWrapper const& other) {
    releaseObj();
    m_obj = other.m_obj;
    acquireObj();
    return *this;
  }

  void unlink() {
    std::visit([](auto&& obj) { obj = nullptr; }, m_obj);
  }

  const podio::ObjectID getObjectID() const {
    return std::visit([](auto&& obj) { return obj->id; }, m_obj);
  }

  unsigned int id() const {
    return std::visit([](auto&& obj) {
      const auto objId = obj->id;
      return objId.collectionID * 10000000 + objId.index;
    }, m_obj);
  }

  template<typename U,
           typename = EnableIfValueType<U>>
  bool isCurrentType() const {
    return std::holds_alternative<detail::GetObjPtrT<U>>(m_obj);
  }

  /// Get the wrapped value as user data type.
  /// This overload with access to mutable types only exists for Mutable == true
  /// classes
  template<typename U,
           typename = detail::EnableIfAnyOfAnd<Mutable,
                                               U,
                                               WrappedTypes...,
                                               detail::GetConstT<WrappedTypes>...>>
  U getValue() {
    const auto obj = std::get<detail::GetObjPtrT<U>>(m_obj);
    return U(obj);
  }

  /// Access to the const wrapped user data type
  template<typename U,
           typename = detail::EnableIfAnyOf<U, detail::GetConstT<WrappedTypes>...>>
  U getValue() const {
    const auto obj = std::get<detail::GetObjPtrT<U>>(m_obj);
    return U(obj);
  }

protected:
  VariantT m_obj;

private:
  void acquireObj() {
    std::visit([](auto&& obj) { obj->acquire(); }, m_obj);
  }
  void releaseObj() {
    std::visit([](auto&& obj) {
      if(obj) obj->release();
    }, m_obj);
  }
};


}

#endif
