#ifndef PODIO_READOPTIONS_H
#define PODIO_READOPTIONS_H

#include <string>
#include <vector>

namespace podio {
struct ReadOptions {
  std::vector<std::string> collsToRead{};
  bool skipUnredable{false};

  inline static ReadOptions SkipUnreadable() {
    return {{}, true};
  }

  inline static ReadOptions Only(std::vector<std::string> collections) {
    return {std::move(collections), false};
  }
};
} // namespace podio

#endif
