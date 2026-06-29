#include "podio/utilities/ArrowUtils.h"
#include <arrow/status.h>
#include <stdexcept>

namespace podio::arrow_utils {

void checkStatus(const arrow::Status& status, const std::string& msg) {
  if (!status.ok()) {
    throw std::runtime_error("Arrow error: " + msg + ": " + status.ToString());
  }
}

} // namespace podio::arrow_utils
