#ifndef podioVersion_h
#define podioVersion_h

#include <cstdint>
#include <tuple>
#include <ostream>
#if __cplusplus >= 202002L
#include <compare>
#endif

#define podio_VERSION @podio_VERSION@
#define podio_VERSION_MAJOR @podio_VERSION_MAJOR@
#define podio_VERSION_MINOR @podio_VERSION_MINOR@
#define podio_VERSION_PATCH @podio_VERSION_PATCH@

namespace podio::version {

  struct Version {
    uint16_t major{0};
    uint16_t minor{0};
    uint16_t patch{0};

#if __cplusplus >= 202002L
    auto operator<=>(const Version&) const = default;
#else
// No spaceship yet in c++17
#define DEFINE_COMP_OPERATOR(OP)                                                   \
    constexpr bool operator OP(const Version& o) const noexcept {                  \
      return std::tie(major, minor, patch) OP std::tie(o.major, o.minor, o.patch); \
    }

    DEFINE_COMP_OPERATOR(<)
    DEFINE_COMP_OPERATOR(<=)
    DEFINE_COMP_OPERATOR(>)
    DEFINE_COMP_OPERATOR(>=)
    DEFINE_COMP_OPERATOR(==)
    DEFINE_COMP_OPERATOR(!=)

#undef DEFINE_COMP_OPERATOR
#endif

    friend std::ostream& operator<<(std::ostream&, const Version& v);
  };

  inline std::ostream& operator<<(std::ostream& os, const Version& v) {
    return os << v.major << "." << v.minor << "." << v.patch;
  }

  /**
   * The current build version
   */
  static constexpr Version build_version{@podio_VERSION_MAJOR@, @podio_VERSION_MINOR@, @podio_VERSION_PATCH@};

  enum class Compatibility {
    AnyNewer,  ///< A version is equal or higher than another version
    SameMajor, ///< Two versions have the same major version
    SameMinor, ///< Two versions have the same major and minor version
    Exact      ///< Two versions are exactly the same
  };

  /**
   * Check if Version va is compatible with Version vb under a given
   * compatibility strategy (defaults AnyNewer).
   */
  inline constexpr bool compatible(Version va, Version vb, Compatibility compat=Compatibility::AnyNewer) noexcept {
    switch (compat) {
      case Compatibility::Exact:
        return va == vb;
      case Compatibility::AnyNewer:
        return va >= vb;
      case Compatibility::SameMajor:
        return va.major == vb.major;
      case Compatibility::SameMinor:
        return va.major == vb.major && va.minor == vb.minor;
    }
  }

  /**
   * Check if the version is compatible with the current build_version
   */
  inline constexpr bool compatible(Version v, Compatibility compat=Compatibility::AnyNewer) {
    return compatible(v, build_version, compat);
  }
   
}


#endif
