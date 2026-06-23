#ifndef PODIO_ARROWTYPEREGISTRY_H
#define PODIO_ARROWTYPEREGISTRY_H

#include <memory>
#include <string>
#include <unordered_map>

// Forward declaration of arrow::DataType to avoid including Arrow headers globally
namespace arrow {
class DataType;
}

namespace podio {

/**
 * @brief Global singleton registry mapping PODIO type name strings to their
 * corresponding Apache Arrow DataTypes.
 */
class ArrowTypeRegistry {
public:
  ArrowTypeRegistry(const ArrowTypeRegistry&) = delete;
  ArrowTypeRegistry& operator=(const ArrowTypeRegistry&) = delete;
  ArrowTypeRegistry(ArrowTypeRegistry&&) = delete;
  ArrowTypeRegistry& operator=(ArrowTypeRegistry&&) = delete;
  ~ArrowTypeRegistry() = default;

  /// Get the mutable singleton instance for registration
  static ArrowTypeRegistry& mutInstance();

  /// Get the read-only singleton instance
  static ArrowTypeRegistry const& instance();

  /**
   * @brief Register an Arrow DataType for a specific type name.
   */
  void registerType(const std::string& typeName, std::shared_ptr<arrow::DataType> type);

  /**
   * @brief Retrieve the Arrow DataType registered for a specific type name.
   * @return The registered DataType, or nullptr if not registered.
   */
  std::shared_ptr<arrow::DataType> getType(const std::string& typeName) const;

private:
  ArrowTypeRegistry() : m_registry() {
  }

  std::unordered_map<std::string, std::shared_ptr<arrow::DataType>> m_registry;
};

} // namespace podio

#endif // PODIO_ARROWTYPEREGISTRY_H
