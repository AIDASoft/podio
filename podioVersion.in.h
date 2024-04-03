#ifndef PODIO_PODIOVERSION_H
#define PODIO_PODIOVERSION_H

#include <cstdint>
#include <sstream>
#include <ostream>
#include <tuple>
#if __cplusplus >= 202002L
  #include <compare>
#endif

// Some preprocessor constants and macros for the use cases where they might be
// necessary

/// Define a version to be used in podio.
#define PODIO_VERSION(major, minor, patch)                                                                             \
  ((UINT64_C(major) << 32) | (UINT64_C(minor) << 16) | UINT64_C(patch))
/// Get the major version from a preprocessor defined version
#define PODIO_MAJOR_VERSION(v) (((v) & (-1UL >> 16)) >> 32)
/// Get the minor version from a preprocessor defined version
#define PODIO_MINOR_VERSION(v) (((v) & (-1UL >> 32)) >> 16)
/// Get the patch version from a preprocessor defined version
#define PODIO_PATCH_VERSION(v) ((v) & (-1UL >> 48))

// Some helper constants that are populated by the cmake configure step
#define podio_VERSION_MAJOR @podio_VERSION_MAJOR@
#define podio_VERSION_MINOR @podio_VERSION_MINOR@
#define podio_VERSION_PATCH @podio_VERSION_PATCH@
#define podio_VERSION PODIO_VERSION(podio_VERSION_MAJOR, podio_VERSION_MINOR, podio_VERSION_PATCH)

/// The encoded version with which podio has been built
#define PODIO_BUILD_VERSION PODIO_VERSION(podio_VERSION_MAJOR, podio_VERSION_MINOR, podio_VERSION_PATCH)

namespace podio::version {

/// Version class consisting of three 16 bit unsigned integers to hold the major,
/// minor and patch version. Provides constexpr comparison operators that allow
/// one to use this class in constexpr-if clauses.
struct Version {
  uint16_t major{0};
  uint16_t minor{0};
  uint16_t patch{0};

#if __cplusplus >= 202002L
  auto operator<=>(const Version&) const = default;
#else
  // No spaceship yet in c++17
  #define DEFINE_COMP_OPERATOR(OP)                                                                                     \
    constexpr bool operator OP(const Version& o) const noexcept {                                                      \
      return std::tie(major, minor, patch) OP std::tie(o.major, o.minor, o.patch);                                     \
    }

  DEFINE_COMP_OPERATOR(<)
  DEFINE_COMP_OPERATOR(<=)
  DEFINE_COMP_OPERATOR(>)
  DEFINE_COMP_OPERATOR(>=)
  DEFINE_COMP_OPERATOR(==)
  DEFINE_COMP_OPERATOR(!=)

  #undef DEFINE_COMP_OPERATOR
#endif

  explicit operator std::string() const {
    std::stringstream ss;
    ss << *this;
    return ss.str();
  };

  friend std::ostream& operator<<(std::ostream&, const Version& v);
};

inline std::ostream& operator<<(std::ostream& os, const Version& v) {
  return os << v.major << "." << v.minor << "." << v.patch;
}

/// The current build version
static constexpr Version build_version{podio_VERSION_MAJOR, podio_VERSION_MINOR, podio_VERSION_PATCH};

/// Decode a version from a 64 bit unsigned
static constexpr Version decode_version(unsigned long version) noexcept {
  return Version{(uint16_t)PODIO_MAJOR_VERSION(version), (uint16_t)PODIO_MINOR_VERSION(version),
                 (uint16_t)PODIO_PATCH_VERSION(version)};
}
} // namespace podio::version

#endif
