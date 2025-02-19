#ifndef PODIO_UTILITIES_TYPEHELPERS_H
#define PODIO_UTILITIES_TYPEHELPERS_H

#include <map>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace podio {
// Implement the minimal feature set we need
namespace det {
  namespace detail {
    template <typename DefT, typename AlwaysVoidT, template <typename...> typename Op, typename... Args>
    struct detector {
      using value_t = std::false_type;
      using type = DefT;
    };

    template <typename DefT, template <typename...> typename Op, typename... Args>
    struct detector<DefT, std::void_t<Op<Args...>>, Op, Args...> {
      using value_t = std::true_type;
      using type = Op<Args...>;
    };
  } // namespace detail

  struct nonesuch {
    ~nonesuch() = delete;
    nonesuch(const nonesuch&) = delete;
    void operator=(const nonesuch&) = delete;
  };

  template <typename DefT, template <typename...> typename Op, typename... Args>
  using detected_or = detail::detector<DefT, void, Op, Args...>;

  template <template <typename...> typename Op, typename... Args>
  constexpr bool is_detected_v = requires { typename Op<Args...>; };

} // namespace det

namespace detail {

  /// Helper struct to determine whether a given type T is in a tuple of types
  /// that act as a type list in this case
  template <typename T, typename>
  struct TypeInTupleHelper : std::false_type {};

  template <typename T, typename... Ts>
  struct TypeInTupleHelper<T, std::tuple<Ts...>> : std::disjunction<std::is_same<T, Ts>...> {};

  /// variable template for determining whether type T is in a tuple with types
  /// Ts
  template <typename T, typename Tuple>
  static constexpr bool isInTuple = TypeInTupleHelper<T, Tuple>::value;

  /// Helper struct to turn a tuple of types into a tuple of a template of types, e.g.
  ///
  /// std::tuple<int, float> -> std::tuple<std::vector<int>, std::vector<float>>
  /// if the passed template is std::vector
  ///
  /// @note making the template template parameter to Template variadic because
  /// clang will not be satisfied otherwise if we use it with, e.g. std::vector.
  /// This will also make root dictionary generation fail. GCC works without this
  /// small workaround and is standard compliant in this case, whereas clang is
  /// not.
  template <template <typename...> typename Template, typename T>
  struct ToTupleOfTemplateHelper;

  template <template <typename...> typename Template, typename... Ts>
  struct ToTupleOfTemplateHelper<Template, std::tuple<Ts...>> {
    using type = std::tuple<Template<Ts>...>;
  };

  /// Type alias to turn a tuple of types into a tuple of vector of types
  template <typename Tuple>
  using TupleOfVector = typename ToTupleOfTemplateHelper<std::vector, Tuple>::type;

  /// Alias template to get the type of a tuple resulting from a concatenation of
  /// tuples
  /// See: https://devblogs.microsoft.com/oldnewthing/20200622-00/?p=103900
  template <typename... Tuples>
  using TupleCatType = decltype(std::tuple_cat(std::declval<Tuples>()...));

  /// variable template for determining whether the type T is in the tuple of all
  /// types or in the tuple of all vector of the passed types
  template <typename T, typename Tuple>
  static constexpr bool isAnyOrVectorOf = isInTuple<T, TupleCatType<Tuple, TupleOfVector<Tuple>>>;

  /// Helper struct to extract the type from a std::vector or return the
  /// original type if it is not a vector. Works only for "simple" types and does
  /// not strip const-ness
  template <typename T>
  struct GetVectorTypeHelper {
    using type = T;
  };

  template <typename T>
  struct GetVectorTypeHelper<std::vector<T>> {
    using type = T;
  };

  template <typename T>
  using GetVectorType = typename GetVectorTypeHelper<T>::type;

  /// Helper struct to detect whether a type is a std::vector
  template <typename T>
  struct IsVectorHelper : std::false_type {};

  template <typename T>
  struct IsVectorHelper<std::vector<T>> : std::true_type {};

  /// Alias template for deciding whether the passed type T is a vector or not
  template <typename T>
  static constexpr bool isVector = IsVectorHelper<T>::value;

  /// Helper struct to detect whether a type is a std::map or std::unordered_map
  template <typename T>
  struct IsMapHelper : std::false_type {};

  template <typename K, typename V>
  struct IsMapHelper<std::map<K, V>> : std::true_type {};

  template <typename K, typename V>
  struct IsMapHelper<std::unordered_map<K, V>> : std::true_type {};

  /// Alias template for deciding whether the passed type T is a map or
  /// unordered_map
  template <typename T>
  static constexpr bool isMap = IsMapHelper<T>::value;

  /// Helper struct to homogenize the (type) access for things that behave like
  /// maps, e.g. vectors of pairs (and obviously maps).
  ///
  /// @note This is not SFINAE friendly.
  template <typename T, typename IsMap = std::bool_constant<isMap<T>>,
            typename IsVector = std::bool_constant<isVector<T> && (std::tuple_size<typename T::value_type>() == 2)>>
  struct MapLikeTypeHelper {};

  /// Specialization for actual maps
  template <typename T>
  struct MapLikeTypeHelper<T, std::bool_constant<true>, std::bool_constant<false>> {
    using key_type = typename T::key_type;
    using mapped_type = typename T::mapped_type;
  };

  /// Specialization for vector of pairs / tuples (of size 2)
  template <typename T>
  struct MapLikeTypeHelper<T, std::bool_constant<false>, std::bool_constant<true>> {
    using key_type = typename std::tuple_element<0, typename T::value_type>::type;
    using mapped_type = typename std::tuple_element<1, typename T::value_type>::type;
  };

  /// Type aliases for easier usage in actual code
  template <typename T>
  using GetKeyType = typename MapLikeTypeHelper<T>::key_type;

  template <typename T>
  using GetMappedType = typename MapLikeTypeHelper<T>::mapped_type;

  /// Detector for checking the existence of a mutable_type type member. Used to
  /// determine whether T is (or could be) a podio generated default (immutable)
  /// handle.
  template <typename T>
  using hasMutable_t = typename T::mutable_type;

  /// Detector for checking the existence of an object_type type member. Used to
  /// determine whether T is (or could be) a podio generated mutable handle.
  template <typename T>
  using hasObject_t = typename T::object_type;

  /// Variable template for determining whether type T is a podio generated
  /// mutable handle class
  template <typename T>
  constexpr static bool isMutableHandleType = det::is_detected_v<hasObject_t, std::remove_reference_t<T>>;

  /// Variable template for determining whether type T is a podio generated
  /// default handle class
  template <typename T>
  constexpr static bool isDefaultHandleType = det::is_detected_v<hasMutable_t, std::remove_reference_t<T>>;

  /// Variable template for obtaining the default handle type from any podio
  /// generated handle type.
  ///
  /// If T is already a default handle, this will return T, if T is a mutable
  /// handle it will return T::object_type.
  template <typename T>
  using GetDefaultHandleType =
      typename det::detected_or<std::remove_reference_t<T>, hasObject_t, std::remove_reference_t<T>>::type;

  /// Variable template for obtaining the mutable handle type from any podio
  /// generated handle type.
  ///
  /// If T is already a mutable handle, this will return T, if T is a default
  /// handle it will return T::mutable_type.
  template <typename T>
  using GetMutableHandleType =
      typename det::detected_or<std::remove_reference_t<T>, hasMutable_t, std::remove_reference_t<T>>::type;

  /// Helper type alias to transform a tuple of handle types to a tuple of
  /// mutable handle types.
  template <typename Tuple>
  using TupleOfMutableTypes = typename ToTupleOfTemplateHelper<GetMutableHandleType, Tuple>::type;

  /// Detector for checking for the existence of an interfaced_type type member
  template <typename T>
  using hasInterface_t = typename T::interfaced_types;

  /// Variable template for checking whether the passed type T is an interface
  /// type.
  ///
  /// @note: This simply checks whether T has an interfaced_types type member.
  template <typename T>
  constexpr static bool isInterfaceType = det::is_detected_v<hasInterface_t, std::remove_reference_t<T>>;

  /// Helper struct to make the detection whether type U can be used to
  /// initialize the interface type T in a SFINAE friendly way
  template <typename T, typename U, typename isInterface = std::bool_constant<isInterfaceType<T>>>
  struct InterfaceInitializerHelper {};

  /// Specialization for actual interface types, including the check whether T
  /// is initializable from U
  template <typename T, typename U>
  struct InterfaceInitializerHelper<T, U, std::bool_constant<true>>
      : std::bool_constant<T::template isInitializableFrom<U>> {};

  /// Specialization for non interface types
  template <typename T, typename U>
  struct InterfaceInitializerHelper<T, U, std::bool_constant<false>> : std::false_type {};

  /// Variable template for checking whether the passed type T is an interface
  /// and can be initialized from type U
  template <typename T, typename U>
  constexpr static bool isInterfaceInitializableFrom = InterfaceInitializerHelper<T, U>::value;

} // namespace detail

// forward declaration to be able to use it below
class CollectionBase;

/// Concept for checking whether a passed type T inherits from podio::CollectionBase
template <typename T>
concept CollectionType = std::is_base_of_v<CollectionBase, T>;

} // namespace podio

#endif // PODIO_UTILITIES_TYPEHELPERS_H
