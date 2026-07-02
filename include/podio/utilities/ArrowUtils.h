#ifndef PODIO_ARROWUTILS_H
#define PODIO_ARROWUTILS_H

#include <string>

namespace arrow {
class Status;
} // namespace arrow

namespace podio::arrow_utils {

/**
 * @brief Check if an arrow::Status is OK. If not, throw std::runtime_error.
 */
void checkStatus(const arrow::Status& status, const std::string& msg);

} // namespace podio::arrow_utils

#endif // PODIO_ARROWUTILS_H
