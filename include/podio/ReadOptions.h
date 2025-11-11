#ifndef PODIO_READOPTIONS_H
#define PODIO_READOPTIONS_H

#include <string>
#include <vector>

namespace podio {

/// Options for configuring read operations
struct ReadOptions {
  /// Collections to read. If empty, all collections will be read
  std::vector<std::string> collsToRead{};
  /// Whether to skip collections that cannot be read
  bool skipUnreadable{false};

  /// Create ReadOptions that skip unreadable collections
  inline static ReadOptions SkipUnreadable() {
    return {{}, true};
  }

  /// Create ReadOptions that only read specific collections
  /// @param collections list of collection names to read
  inline static ReadOptions Only(std::vector<std::string> collections) {
    return {std::move(collections), false};
  }
};
} // namespace podio

#endif
