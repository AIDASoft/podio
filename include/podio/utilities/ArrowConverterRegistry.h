#ifndef PODIO_ARROWCONVERTERREGISTRY_H
#define PODIO_ARROWCONVERTERREGISTRY_H

#include "podio/CollectionBuffers.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace arrow {
class Array;
}

namespace podio {
class CollectionBase;

/**
 * @brief Global singleton registry mapping PODIO type name strings to their
 * corresponding Apache Arrow array converter callbacks.
 */
class ArrowConverterRegistry {
public:
  using CreatorFunc = std::function<std::shared_ptr<arrow::Array>(const podio::CollectionBase*)>;
  using BufferReaderFunc = std::function<podio::CollectionReadBuffers(std::shared_ptr<arrow::Array>, int64_t, bool)>;

  ArrowConverterRegistry(const ArrowConverterRegistry&) = delete;
  ArrowConverterRegistry& operator=(const ArrowConverterRegistry&) = delete;
  ArrowConverterRegistry(ArrowConverterRegistry&&) = delete;
  ArrowConverterRegistry& operator=(ArrowConverterRegistry&&) = delete;
  ~ArrowConverterRegistry() = default;

  /// Get the mutable singleton instance for registration
  static ArrowConverterRegistry& mutInstance();

  /// Get the read-only singleton instance
  static ArrowConverterRegistry const& instance();

  /**
   * @brief Register an Arrow array converter callback for a specific type name.
   */
  void registerConverter(const std::string& typeName, CreatorFunc&& converter);

  /**
   * @brief Retrieve the Arrow collection converter registered for a specific type name.
   * @return The registered CreatorFunc, or nullptr if not registered.
   */
  CreatorFunc getConverter(const std::string& typeName) const;

  /**
   * @brief Register an Arrow array reader callback for a specific type name.
   */
  void registerReader(const std::string& typeName, BufferReaderFunc reader);

  /**
   * @brief Retrieve the Arrow collection reader registered for a specific type name.
   * @return The registered BufferReaderFunc, or nullptr if not registered.
   */
  BufferReaderFunc getReader(const std::string& typeName) const;

private:
  ArrowConverterRegistry() : m_registry() {
  }

  std::unordered_map<std::string, CreatorFunc> m_registry;
  std::unordered_map<std::string, BufferReaderFunc> m_readerRegistry;
};

} // namespace podio

#endif // PODIO_ARROWCONVERTERREGISTRY_H
