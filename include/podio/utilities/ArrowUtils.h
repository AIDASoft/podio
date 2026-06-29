#ifndef PODIO_ARROWUTILS_H
#define PODIO_ARROWUTILS_H

#include <arrow/status.h>
#include <stdexcept>
#include <string>

namespace podio::arrow_utils {

/**
 * @brief Check if an arrow::Status is OK. If not, throw std::runtime_error.
 */
inline void checkStatus(const arrow::Status& status, const std::string& msg) {
  if (!status.ok()) {
    throw std::runtime_error("Arrow error: " + msg + ": " + status.ToString());
  }
}

} // namespace podio::arrow_utils

#endif // PODIO_ARROWUTILS_H
