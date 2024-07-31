#ifndef DATAMODEL_VERSION_H
#define DATAMODEL_VERSION_H

#include "podio/podioVersion.h"

namespace datamodel::ver {
struct V {
  int major{0};
  int minor{0};
  int patch{0};

  explicit constexpr operator podio::version::Version() const noexcept {
    return {static_cast<uint16_t>(major), static_cast<uint16_t>(minor), static_cast<uint16_t>(patch)};
  }
};

constexpr static auto version = V{42, 123, 255};

} // namespace datamodel::ver
#endif
