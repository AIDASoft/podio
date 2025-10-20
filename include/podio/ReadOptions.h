#ifndef PODIO_READOPTIONS_H
#define PODIO_READOPTIONS_H

#include <string>
#include <vector>

namespace podio {
struct ReadOptions {
  std::vector<std::string> collsToRead{};
  bool skipUnredable{false};
};
} // namespace podio

#endif
