#include "datamodel/ExampleHitCollection.h"
#include <catch2/catch_test_macros.hpp>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>

#if __cplusplus >= 202002L
  #include <concepts>
#endif

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
} // namespace traits

TEST_CASE("Collection types", "[collection][container][types][std]") {

  // value_type
  STATIC_REQUIRE(traits::has_value_type_v<CollectionType>);
  // Erasable -allocator aware - mutually exclusive with  Erasable -allocator not aware
  STATIC_REQUIRE_FALSE(traits::has_allocator_type_v<CollectionType>);
  // add check for `std::allocator_traits<A>::destroy(m, p);` expression here
  // STATIC_REQUIRE(...)
  // Erasable -allocator not aware - mutually exclusive // Erasable -allocator aware
  STATIC_REQUIRE(traits::is_erasable_allocator_unaware_v<CollectionType>);

  // reference
  STATIC_REQUIRE_FALSE(traits::has_reference_v<CollectionType>);
  // STATIC_REQUIRE(std::is_same_v<CollectionType::reference, std::add_lvalue_reference_t<CollectionType::value_type>>);

  // const_reference
  STATIC_REQUIRE_FALSE(traits::has_const_reference_v<CollectionType>);
  // STATIC_REQUIRE(std::is_same_v<CollectionType::const_reference,
  //                               std::add_const_t<std::add_lvalue_reference_t<CollectionType::value_type>>>);

  // iterator
  STATIC_REQUIRE(traits::has_iterator_v<CollectionType>);
  STATIC_REQUIRE_FALSE(std::is_convertible_v<CollectionType::iterator, CollectionType::const_iterator>);

  // const_iterator
  STATIC_REQUIRE(traits::has_const_iterator_v<CollectionType>);

  // difference_type
  STATIC_REQUIRE(traits::has_difference_type_v<CollectionType>);

  STATIC_REQUIRE(std::is_signed_v<CollectionType::difference_type>);
  STATIC_REQUIRE(std::is_integral_v<CollectionType::difference_type>);
  STATIC_REQUIRE_FALSE(traits::has_difference_type_v<std::iterator_traits<CollectionType::iterator>>);
  // STATIC_REQUIRE(
  //     std::is_same_v<CollectionType::difference_type,
  //     std::iterator_traits<CollectionType::iterator>::difference_type>);
  STATIC_REQUIRE_FALSE(traits::has_difference_type_v<std::iterator_traits<CollectionType::const_iterator>>);
  // STATIC_REQUIRE(std::is_same_v<CollectionType::difference_type,
  //                               std::iterator_traits<CollectionType::const_iterator>::difference_type>);

  // size_type
  STATIC_REQUIRE(traits::has_size_type_v<CollectionType>);
  STATIC_REQUIRE(std::is_unsigned_v<CollectionType::size_type>);
  STATIC_REQUIRE(std::is_integral_v<CollectionType::size_type>);
  STATIC_REQUIRE(std::numeric_limits<CollectionType::size_type>::max() >=
                 std::numeric_limits<CollectionType::difference_type>::max());
}

TEST_CASE("Collection members", "[collection][container][members][std]") {
  // C()
  STATIC_REQUIRE(std::is_default_constructible_v<CollectionType>);
  REQUIRE(CollectionType().empty() == true);

  // C(a)
  STATIC_REQUIRE_FALSE(std::is_copy_constructible_v<CollectionType>);

  // C(rv)
  STATIC_REQUIRE(std::is_move_constructible_v<CollectionType>);

  // a = b
  STATIC_REQUIRE_FALSE(std::is_copy_assignable_v<CollectionType>);

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
  STATIC_REQUIRE_FALSE(traits::has_equality_comparator_v<CollectionType>);
  // STATIC_REQUIRE(std::is_convertible_v<decltype(std::declval<CollectionType>()==std::declval<CollectionType>()),
  // bool>);

  // a != b
  STATIC_REQUIRE_FALSE(traits::has_inequality_comparator_v<CollectionType>);
  // STATIC_REQUIRE(std::is_convertible_v<decltype(std::declval<CollectionType>()!=std::declval<CollectionType>()),
  // bool>);

  // a.swap(b)
  STATIC_REQUIRE_FALSE(traits::has_swap_v<CollectionType>);
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

TEST_CASE("Collection iterators", "[collection][container][interator][std]") {
  using iterator = CollectionType::iterator;
  using const_iterator = CollectionType::const_iterator;
#if (__cplusplus >= 202002L)
  STATIC_REQUIRE_FALSE(std::forward_iterator<iterator>);
  STATIC_REQUIRE_FALSE(std::forward_iterator<const_iterator>);
#endif

  SECTION("LegacyForwardIterator") {

    SECTION("LegacyInputIterator") {
      SECTION("LegacyIterator") {
        // CopyConstructible
        STATIC_REQUIRE_FALSE(std::is_move_constructible_v<iterator> && std::is_copy_constructible_v<iterator>);

        // CopyAssignable
        STATIC_REQUIRE_FALSE(std::is_move_assignable_v<iterator> && std::is_copy_assignable_v<iterator>);

        // Destructible
        STATIC_REQUIRE(std::is_nothrow_destructible_v<iterator>);

        // Swappable
        STATIC_REQUIRE_FALSE(std::is_swappable_v<iterator>);

#if (__cplusplus < 202002L)
        // std::iterator_traits<It>::value_type
        STATIC_REQUIRE_FALSE(traits::has_value_type_v<std::iterator_traits<iterator>>);
#endif
        // std::iterator_traits<It>::difference_type
        STATIC_REQUIRE_FALSE(traits::has_difference_type_v<std::iterator_traits<iterator>>);
        // std::iterator_traits<It>::reference
        STATIC_REQUIRE_FALSE(traits::has_reference_v<std::iterator_traits<iterator>>);
        // std::iterator_traits<It>::pointer
        STATIC_REQUIRE_FALSE(traits::has_pointer_v<std::iterator_traits<iterator>>);
        // std::iterator_traits<It>::iterator_category
        STATIC_REQUIRE_FALSE(traits::has_iterator_category_v<std::iterator_traits<iterator>>);

        // *r
        FAIL("Not yet implemented");

        // ++r
        FAIL("Not yet implemented");
      }

      // EqualityComparable
      STATIC_REQUIRE(traits::has_equality_comparator_v<iterator>);

      // i != j
      STATIC_REQUIRE(traits::has_inequality_comparator_v<iterator>);

      // *i
      FAIL("Not yet implemented");

      // i->m
      FAIL("Not yet implemented");

      // ++r
      FAIL("Not yet implemented");

      // (void)r++
      FAIL("Not yet implemented");

      //*r++
      STATIC_REQUIRE_FALSE(traits::has_value_type_v<std::iterator_traits<iterator>>);
      // STATIC_REQUIRE(std::is_convertible_v < decltype(*std::declval<iterator>()++),
      //                std::iterator_traits<iterator>::value_type >>);
    }

    // DefaultConstructible
    STATIC_REQUIRE_FALSE(std::is_default_constructible_v<iterator>);
  }

  // multipass guarantee
  FAIL("Not yet implemented");

  // i++
  FAIL("Not yet implemented");

  // *i++
  FAIL("Not yet implemented");
}

TEST_CASE("Collection and std::algorithms", "[collection][container][algorithm][std]") {
  auto a = CollectionType();
  FAIL("Not yet implemented");
}
