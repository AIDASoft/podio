#include "datamodel/ExampleHitCollection.h"
#include <catch2/catch_test_macros.hpp>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>

#if __cplusplus >= 202002L
  #include <concepts>
#endif

// Some of the tests check . If the check annotated with 'REQUIREMENT_NOT_MET' fails, update documentation
// 'doc/collection_as_container.md'
#define REQUIREMENT_NOT_MET(...) STATIC_REQUIRE_FALSE(__VA_ARGS__)

using CollectionType = ExampleHitCollection;

namespace traits {

// typename T::value_type
template <typename T, typename = void>
struct has_value_type : std::false_type {};
template <typename T>
struct has_value_type<T, std::void_t<typename T::value_type>> : std::true_type {};
template <typename T>
inline constexpr bool has_value_type_v = has_value_type<T>::value;

// typename T::reference
template <typename T, typename = void>
struct has_reference : std::false_type {};
template <typename T>
struct has_reference<T, std::void_t<typename T::reference>> : std::true_type {};
template <typename T>
inline constexpr bool has_reference_v = has_reference<T>::value;

// typename T::const_reference
template <typename T, typename = void>
struct has_const_reference : std::false_type {};
template <typename T>
struct has_const_reference<T, std::void_t<typename T::const_reference>> : std::true_type {};
template <typename T>
inline constexpr bool has_const_reference_v = has_const_reference<T>::value;

// typename T::iterator
template <typename T, typename = void>
struct has_iterator : std::false_type {};
template <typename T>
struct has_iterator<T, std::void_t<typename T::iterator>> : std::true_type {};
template <typename T>
inline constexpr bool has_iterator_v = has_iterator<T>::value;

// typename T::const_iterator
template <typename T, typename = void>
struct has_const_iterator : std::false_type {};
template <typename T>
struct has_const_iterator<T, std::void_t<typename T::const_iterator>> : std::true_type {};
template <typename T>
inline constexpr bool has_const_iterator_v = has_const_iterator<T>::value;

// typename T::difference_type
template <typename T, typename = void>
struct has_difference_type : std::false_type {};
template <typename T>
struct has_difference_type<T, std::void_t<typename T::difference_type>> : std::true_type {};
template <typename T>
inline constexpr bool has_difference_type_v = has_difference_type<T>::value;

// typename T::size_type
template <typename T, typename = void>
struct has_size_type : std::false_type {};
template <typename T>
struct has_size_type<T, std::void_t<typename T::size_type>> : std::true_type {};
template <typename T>
inline constexpr bool has_size_type_v = has_size_type<T>::value;

// typename T::pointer
template <typename T, typename = void>
struct has_pointer : std::false_type {};
template <typename T>
struct has_pointer<T, std::void_t<typename T::pointer>> : std::true_type {};
template <typename T>
inline constexpr bool has_pointer_v = has_pointer<T>::value;

// typename T::allocator_type
template <typename T, typename = void>
struct has_allocator_type : std::false_type {};
template <typename T>
struct has_allocator_type<T, std::void_t<typename T::allocator_type>> : std::true_type {};
template <typename T>
inline constexpr bool has_allocator_type_v = has_allocator_type<T>::value;

// is_erasable_allocator_unaware
template <class, class = void>
struct is_erasable_allocator_unaware : std::false_type {};
template <class T>
struct is_erasable_allocator_unaware<
    T,
    std::void_t<decltype(std::allocator_traits<std::allocator<typename T::value_type>>::destroy(
        std::declval<std::add_lvalue_reference_t<std::allocator<typename T::value_type>>>(),
        std::declval<std::add_pointer_t<typename T::value_type>>()))>> : std::true_type {};
template <typename T>
inline constexpr bool is_erasable_allocator_unaware_v = is_erasable_allocator_unaware<T>::value;

// typename T::iterator_category
template <typename T, typename = void>
struct has_iterator_category : std::false_type {};
template <typename T>
struct has_iterator_category<T, std::void_t<typename T::iterator_category>> : std::true_type {};
template <typename T>
inline constexpr bool has_iterator_category_v = has_iterator_category<T>::value;

// T::begin
template <class, class = void>
struct has_begin : std::false_type {};
template <class T>
struct has_begin<T, std::void_t<decltype(std::declval<T>().begin())>> : std::true_type {};
template <typename T>
inline constexpr bool has_begin_v = has_begin<T>::value;

// T::end
template <class, class = void>
struct has_end : std::false_type {};
template <class T>
struct has_end<T, std::void_t<decltype(std::declval<T>().end())>> : std::true_type {};
template <typename T>
inline constexpr bool has_end_v = has_end<T>::value;

// T::cbegin
template <class, class = void>
struct has_cbegin : std::false_type {};
template <class T>
struct has_cbegin<T, std::void_t<decltype(std::declval<T>().cbegin())>> : std::true_type {};
template <typename T>
inline constexpr bool has_cbegin_v = has_cbegin<T>::value;

// T::cend
template <class, class = void>
struct has_cend : std::false_type {};
template <class T>
struct has_cend<T, std::void_t<decltype(std::declval<T>().cend())>> : std::true_type {};
template <typename T>
inline constexpr bool has_cend_v = has_cend<T>::value;

// T::operator==(T)
template <class, class = void>
struct has_equality_comparator : std::false_type {};
template <class T>
struct has_equality_comparator<T, std::void_t<decltype(std::declval<T>() == std::declval<T>())>> : std::true_type {};
template <typename T>
inline constexpr bool has_equality_comparator_v = has_equality_comparator<T>::value;

// T::operator!=(T)
template <class, class = void>
struct has_inequality_comparator : std::false_type {};
template <class T>
struct has_inequality_comparator<T, std::void_t<decltype(std::declval<T>() != std::declval<T>())>> : std::true_type {};
template <typename T>
inline constexpr bool has_inequality_comparator_v = has_inequality_comparator<T>::value;

// T::swap
template <class, class = void>
struct has_swap : std::false_type {};
template <class T>
struct has_swap<T, std::void_t<decltype(std::declval<T>().swap(std::declval<T>()))>> : std::true_type {};
template <typename T>
inline constexpr bool has_swap_v = has_swap<T>::value;

// T::size
template <class, class = void>
struct has_size : std::false_type {};
template <class T>
struct has_size<T, std::void_t<decltype(std::declval<T>().size())>> : std::true_type {};
template <typename T>
inline constexpr bool has_size_v = has_size<T>::value;

// T::max_size
template <class, class = void>
struct has_max_size : std::false_type {};
template <class T>
struct has_max_size<T, std::void_t<decltype(std::declval<T>().max_size())>> : std::true_type {};
template <typename T>
inline constexpr bool has_max_size_v = has_max_size<T>::value;

// T::empty
template <class, class = void>
struct has_empty : std::false_type {};
template <class T>
struct has_empty<T, std::void_t<decltype(std::declval<T>().empty())>> : std::true_type {};
template <typename T>
inline constexpr bool has_empty_v = has_empty<T>::value;

// T::operator++() (preincrement)
template <class, class = void>
struct has_preincrement : std::false_type {};
template <class T>
struct has_preincrement<T, std::void_t<decltype(++std::declval<T>())>> : std::true_type {};
template <typename T>
inline constexpr bool has_preincrement_v = has_preincrement<T>::value;

// T::operator++(int) (postincrement)
template <class, class = void>
struct has_postincrement : std::false_type {};
template <class T>
struct has_postincrement<T, std::void_t<decltype(std::declval<T>()++)>> : std::true_type {};
template <typename T>
inline constexpr bool has_postincrement_v = has_postincrement<T>::value;
} // namespace traits

TEST_CASE("Collection container types", "[collection][container][types][std]") {

  // value_type
  STATIC_REQUIRE(traits::has_value_type_v<CollectionType>);
  // Erasable -allocator aware - mutually exclusive with  Erasable -allocator not aware
  REQUIREMENT_NOT_MET(traits::has_allocator_type_v<CollectionType>);
  // add check for `std::allocator_traits<A>::destroy(m, p);` expression here
  // STATIC_REQUIRE(...)
  // Erasable -allocator not aware - mutually exclusive // Erasable -allocator aware
  STATIC_REQUIRE(traits::is_erasable_allocator_unaware_v<CollectionType>);

  // reference
  REQUIREMENT_NOT_MET(traits::has_reference_v<CollectionType>);
  // STATIC_REQUIRE(std::is_same_v<CollectionType::reference, std::add_lvalue_reference_t<CollectionType::value_type>>);

  // const_reference
  REQUIREMENT_NOT_MET(traits::has_const_reference_v<CollectionType>); // The check will fail once the support is added.
                                                                      // In that case replace it with STATIC_REQUIRE,
                                                                      // uncomment checks immediately below, and update
                                                                      // documentation
  // STATIC_REQUIRE(std::is_same_v<CollectionType::const_reference,
  //                               std::add_const_t<std::add_lvalue_reference_t<CollectionType::value_type>>>);

  // iterator
  STATIC_REQUIRE(traits::has_iterator_v<CollectionType>);
  REQUIREMENT_NOT_MET(std::is_convertible_v<CollectionType::iterator, CollectionType::const_iterator>);

  // const_iterator
  STATIC_REQUIRE(traits::has_const_iterator_v<CollectionType>);

  // difference_type
  STATIC_REQUIRE(traits::has_difference_type_v<CollectionType>);
  STATIC_REQUIRE(std::is_signed_v<CollectionType::difference_type>);
  STATIC_REQUIRE(std::is_integral_v<CollectionType::difference_type>);
  REQUIREMENT_NOT_MET(traits::has_difference_type_v<std::iterator_traits<CollectionType::iterator>>);
  // STATIC_REQUIRE(
  //     std::is_same_v<CollectionType::difference_type,
  //     std::iterator_traits<CollectionType::iterator>::difference_type>);
  REQUIREMENT_NOT_MET(traits::has_difference_type_v<std::iterator_traits<CollectionType::const_iterator>>);
  // STATIC_REQUIRE(std::is_same_v<CollectionType::difference_type,
  //                               std::iterator_traits<CollectionType::const_iterator>::difference_type>);

  // size_type
  STATIC_REQUIRE(traits::has_size_type_v<CollectionType>);
  STATIC_REQUIRE(std::is_unsigned_v<CollectionType::size_type>);
  STATIC_REQUIRE(std::is_integral_v<CollectionType::size_type>);
  STATIC_REQUIRE(std::numeric_limits<CollectionType::size_type>::max() >=
                 std::numeric_limits<CollectionType::difference_type>::max());
}

TEST_CASE("Collection container members", "[collection][container][members][std]") {
  // C()
  STATIC_REQUIRE(std::is_default_constructible_v<CollectionType>);
  REQUIRE(CollectionType().empty() == true);

  // C(a)
  REQUIREMENT_NOT_MET(std::is_copy_constructible_v<CollectionType>);

  // C(rv)
  STATIC_REQUIRE(std::is_move_constructible_v<CollectionType>);

  // a = b
  REQUIREMENT_NOT_MET(std::is_copy_assignable_v<CollectionType>);

  // a = rv
  STATIC_REQUIRE(std::is_move_assignable_v<CollectionType>);

  // a.~C()
  STATIC_REQUIRE(std::is_destructible_v<CollectionType>);

  // a.begin()
  STATIC_REQUIRE(traits::has_begin_v<CollectionType>);
  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<CollectionType>().begin()), CollectionType::iterator>);
  STATIC_REQUIRE(
      std::is_same_v<decltype(std::declval<const CollectionType>().begin()), CollectionType::const_iterator>);

  // a.end()
  STATIC_REQUIRE(traits::has_end_v<CollectionType>);
  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<CollectionType>().end()), CollectionType::iterator>);
  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<const CollectionType>().end()), CollectionType::const_iterator>);

  // a.cbegin()
  STATIC_REQUIRE(traits::has_cbegin_v<CollectionType>);
  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<CollectionType>().cbegin()), CollectionType::const_iterator>);

  // a.cend()
  STATIC_REQUIRE(traits::has_cend_v<CollectionType>);
  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<CollectionType>().cend()), CollectionType::const_iterator>);

  // a == b
  REQUIREMENT_NOT_MET(traits::has_equality_comparator_v<CollectionType>);
  // STATIC_REQUIRE(std::is_convertible_v<decltype(std::declval<CollectionType>()==std::declval<CollectionType>()),
  // bool>);
  // value_type is EqualityComparable
  STATIC_REQUIRE(traits::has_equality_comparator_v<CollectionType::value_type>);
  STATIC_REQUIRE(
      std::is_convertible_v<
          decltype(std::declval<CollectionType::value_type>() != std::declval<CollectionType::value_type>()), bool>);

  // a != b
  REQUIREMENT_NOT_MET(traits::has_inequality_comparator_v<CollectionType>);
  // STATIC_REQUIRE(std::is_convertible_v<decltype(std::declval<CollectionType>()!=std::declval<CollectionType>()),
  // bool>);

  // a.swap(b)
  REQUIREMENT_NOT_MET(traits::has_swap_v<CollectionType>);
  // STATIC_REQUIRE(
  //     std::is_same_v<decltype(std::declval<CollectionType>().swap(std::declval<CollectionType>())), void>);

  // swap(a,b)
  STATIC_REQUIRE(std::is_swappable_v<CollectionType>);

  // a.size()
  STATIC_REQUIRE(traits::has_size_v<CollectionType>);
  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<CollectionType>().size()), CollectionType::size_type>);

  // a.max_size())
  STATIC_REQUIRE(traits::has_max_size_v<CollectionType>);
  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<CollectionType>().max_size()), CollectionType::size_type>);

  // a.empty()
  STATIC_REQUIRE(traits::has_empty_v<CollectionType>);
  STATIC_REQUIRE(std::is_convertible_v<decltype(std::declval<CollectionType>().empty()), bool>);
}

TEST_CASE("Collection AllocatorAwareContainer types", "[collection][container][types][std]") {
  REQUIREMENT_NOT_MET(traits::has_allocator_type_v<CollectionType>);
}
// TODO add tests for AllocatorAwareContainer statements and expressions

TEST_CASE("Collection iterators", "[collection][container][interator][std]") {
  using iterator = CollectionType::iterator;
  using const_iterator = CollectionType::const_iterator;

  SECTION("LegacyForwardIterator") {
#if (__cplusplus >= 202002L)
    REQUIREMENT_NOT_MET(std::forward_iterator<iterator>);
    REQUIREMENT_NOT_MET(std::forward_iterator<const_iterator>);
#endif

    SECTION("LegacyInputIterator") {
#if (__cplusplus >= 202002L)
      REQUIREMENT_NOT_MET(std::input_iterator<iterator>);
      REQUIREMENT_NOT_MET(std::input_iterator<const_iterator>);
#endif

      SECTION("LegacyIterator") {
#if (__cplusplus >= 202002L)
        REQUIREMENT_NOT_MET(std::input_or_output_iterator<iterator>);
        REQUIREMENT_NOT_MET(std::input_or_output_iterator<const_iterator>);
#endif
        // CopyConstructible
        REQUIREMENT_NOT_MET(std::is_move_constructible_v<iterator>);
        REQUIREMENT_NOT_MET(std::is_copy_constructible_v<iterator>);

        // CopyAssignable
        REQUIREMENT_NOT_MET(std::is_move_assignable_v<iterator>);
        REQUIREMENT_NOT_MET(std::is_copy_assignable_v<iterator>);

        // Destructible
        STATIC_REQUIRE(std::is_nothrow_destructible_v<iterator>);

        // Swappable
        REQUIREMENT_NOT_MET(std::is_swappable_v<iterator>);

#if (__cplusplus < 202002L)
        // std::iterator_traits<It>::value_type
        REQUIREMENT_NOT_MET(traits::has_value_type_v<std::iterator_traits<iterator>>);
#endif
        // std::iterator_traits<It>::difference_type
        REQUIREMENT_NOT_MET(traits::has_difference_type_v<std::iterator_traits<iterator>>);
        // std::iterator_traits<It>::reference
        REQUIREMENT_NOT_MET(traits::has_reference_v<std::iterator_traits<iterator>>);
        // std::iterator_traits<It>::pointer
        REQUIREMENT_NOT_MET(traits::has_pointer_v<std::iterator_traits<iterator>>);
        // std::iterator_traits<It>::iterator_category
        REQUIREMENT_NOT_MET(traits::has_iterator_category_v<std::iterator_traits<iterator>>);
        // *r
        STATIC_REQUIRE(!std::is_same_v<void, decltype(*std::declval<CollectionType::iterator>())>);
        // ++r
        STATIC_REQUIRE(traits::has_preincrement_v<iterator>);
        STATIC_REQUIRE(std::is_same_v<decltype(++std::declval<CollectionType::iterator>()),
                                      std::add_lvalue_reference_t<CollectionType::iterator>>);
      }

      // EqualityComparable
      STATIC_REQUIRE(traits::has_equality_comparator_v<iterator>);
      STATIC_REQUIRE(std::is_convertible_v<decltype(std::declval<iterator>() != std::declval<iterator>()), bool>);

      // i != j
      STATIC_REQUIRE(traits::has_inequality_comparator_v<iterator>);
      STATIC_REQUIRE(std::is_constructible_v<bool, decltype(std::declval<iterator>() != std::declval<iterator>())>);

      // *i
      REQUIREMENT_NOT_MET(traits::has_reference_v<iterator>);
      // STATIC_REQUIRE(!std::is_same_v<std::iterator_traits<iterator>::reference,
      // decltype(*std::declval<CollectionType::iterator>())>);
      REQUIREMENT_NOT_MET(traits::has_value_type_v<iterator>);
      // STATIC_REQUIRE(!std::is_convertible_v<decltype(*std::declval<CollectionType::iterator>()),
      // std::iterator_traits<iterator>::value_type>);

      // i->m
      STATIC_REQUIRE(
          std::is_same_v<decltype(std::declval<iterator>()->energy()), decltype((*std::declval<iterator>()).energy())>);

      // ++r
      STATIC_REQUIRE(traits::has_preincrement_v<iterator>);
      STATIC_REQUIRE(std::is_same_v<decltype(++std::declval<CollectionType::iterator>()),
                                    std::add_lvalue_reference_t<CollectionType::iterator>>);

      // (void)r++
      REQUIREMENT_NOT_MET(traits::has_postincrement_v<iterator>);
      STATIC_REQUIRE(traits::has_preincrement_v<iterator>);
      REQUIREMENT_NOT_MET(traits::has_value_type_v<std::iterator_traits<iterator>>);
      // STATIC_REQUIRE(std::is_same_v<decltype((void)++std::declval<iterator>()),
      // decltype((void)std::declval<iterator>()++)>);

      //*r++
      REQUIREMENT_NOT_MET(traits::has_postincrement_v<iterator>);
      REQUIREMENT_NOT_MET(traits::has_value_type_v<std::iterator_traits<iterator>>);
      // STATIC_REQUIRE(std::is_convertible_v < decltype(*std::declval<iterator>()++),
      //                std::iterator_traits<iterator>::value_type >>);
    }

    // Mutable iterator: reference same as value_type& or value_type&&
    REQUIREMENT_NOT_MET(traits::has_reference_v<iterator>);
    REQUIREMENT_NOT_MET(traits::has_value_type_v<iterator>);
    // STATIC_REQUIRE(std::is_same_v<std::iterator_traits<iterator>::reference,
    //                               std::add_lvalue_reference_t<std::iterator_traits<iterator>::value_type>> ||
    //                std::is_same_v<std::iterator_traits<iterator>::reference,
    //                               std::add_rvalue_reference_t<std::iterator_traits<iterator>::value_type>>);

    // Immutable iterator: reference same as const value_type& or const value_type&&
    REQUIREMENT_NOT_MET(traits::has_reference_v<const_iterator>);
    REQUIREMENT_NOT_MET(traits::has_value_type_v<const_iterator>);
    // STATIC_REQUIRE(std::is_same_v<std::iterator_traits<const_iterator>::reference,
    //                               std::add_const_t<std::add_lvalue_reference_t<std::iterator_traits<const_iterator>::value_type>>>
    //                               ||
    //                std::is_same_v<std::iterator_traits<const_iterator>::reference,
    //                               std::add_const_t<std::add_rvalue_reference_t<std::iterator_traits<const_iterator>::value_type>>>);

    // DefaultConstructible
    REQUIREMENT_NOT_MET(std::is_default_constructible_v<iterator>);
  }

  // Multipass guarantee
  {
    CollectionType coll;
    for (int i = 0; i < 3; ++i) {
      coll.create();
    }
    auto a = coll.begin();
    auto b = coll.begin();
    REQUIRE(a == b);
    REQUIRE(*a == *b);
    REQUIRE(++a == ++b);
    REQUIRE(*a == *b);
    REQUIREMENT_NOT_MET(std::is_copy_constructible_v<iterator>);
    // auto a_copy = a;
    // ++a_copy;
    // REQUIRE(a == b);
    // REQUIRE(*a == *b);
  }

  // Singular iterators
  STATIC_REQUIRE(traits::has_equality_comparator_v<iterator>);
  REQUIREMENT_NOT_MET(std::is_default_constructible_v<iterator>);
  //{
  //  CollectionType some_collection{};
  //  REQUIRE(iterator{} == some_collection.end());
  //  REQUIRE(iterator{} == iterator{});
  //}

  // i++
  REQUIREMENT_NOT_MET(traits::has_postincrement_v<iterator>);
  // STATIC_REQUIRE(std::is_same_v<decltype(std::declval<CollectionType::iterator>()++), CollectionType::iterator>);

  // *i++
  REQUIREMENT_NOT_MET(traits::has_postincrement_v<iterator>);
  REQUIREMENT_NOT_MET(traits::has_reference_v<std::iterator_traits<iterator>>);
  // STATIC_REQUIRE(std::is_same_v < decltype(*std::declval<iterator>()++),
  //                 std::iterator_traits<iterator>::reference >>);

  SECTION("LegacyOutputIterator") {
#if (__cplusplus >= 202002L)
    REQUIREMENT_NOT_MET(std::output_iterator<iterator, CollectionType::value_type>);
    REQUIREMENT_NOT_MET(std::output_iterator<const_iterator, CollectionType::value_type>);
#endif

    // is class type or pointer type
    STATIC_REQUIRE(std::is_pointer_v<iterator> || std::is_class_v<iterator>);
    STATIC_REQUIRE(std::is_pointer_v<const_iterator> || std::is_class_v<const_iterator>);

    // *r = o
    FAIL("Not implemented yet");

    // ++r
    STATIC_REQUIRE(traits::has_preincrement_v<iterator>);
    STATIC_REQUIRE(std::is_same_v<decltype(++std::declval<iterator>()), std::add_lvalue_reference_t<iterator>>);

    // r++
    REQUIREMENT_NOT_MET(traits::has_postincrement_v<iterator>);
    // STATIC_REQUIRE(std::is_convertible_v<decltype(std::declval<iterator>()++),
    // std::add_const_t<std::add_lvalue_reference_t<iterator>>>);

    //*r++ =o
    FAIL("Not implemented yet");
  }
}

TEST_CASE("Collection and std iterator adaptors", "[collection][container][adapter][std]") {
  auto a = CollectionType();
  FAIL("Not yet implemented");
}

TEST_CASE("Collection and std::algorithms", "[collection][container][algorithm][std]") {
  auto a = CollectionType();
  FAIL("Not yet implemented");
}

#undef REQUIREMENT_NOT_MET