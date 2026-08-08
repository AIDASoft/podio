#ifndef PODIO_ARROWCONVERTERREGISTRY_H
#define PODIO_ARROWCONVERTERREGISTRY_H

#include "podio/CollectionBuffers.h"
#include "podio/SchemaEvolution.h"
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>

namespace arrow {
class Array;
}

namespace podio {
class CollectionBase;

/**
 * @brief Global singleton registry mapping PODIO type name strings to their
 * corresponding Apache Arrow array converter callbacks.
 *
 * Registration happens lazily on the first call to getConverter or getReader,
 * when the necessary datamodel-specific Arrow converter libraries (e.g.,
 * libpodioArrow.so, libedm4hepArrow.so) are loaded. It is expected that this
 * happens before worker threads query the registry. Once populated, the registry
 * is read-only and can be safely accessed from multiple threads concurrently.
 */
class ArrowConverterRegistry {
public:
  using CreatorFunc = std::function<std::shared_ptr<arrow::Array>(const podio::CollectionBase*)>;
  using BufferReaderFunc = std::function<std::optional<podio::CollectionReadBuffers>(std::shared_ptr<arrow::Array>,
                                                                                     int64_t, bool, SchemaVersionT)>;

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
  void registerReader(const std::string& typeName, BufferReaderFunc&& reader);

  /**
   * @brief Retrieve the Arrow collection reader registered for a specific type name.
   * @return The registered BufferReaderFunc, or nullptr if not registered.
   */
  BufferReaderFunc getReader(const std::string& typeName) const;

private:
  ArrowConverterRegistry() : m_registry(), m_readerRegistry() {
  }

  std::map<std::string, CreatorFunc> m_registry;
  std::map<std::string, BufferReaderFunc> m_readerRegistry;
};

class ArrowConverterLibraryLoader {
private:
  ArrowConverterLibraryLoader();

  enum class LoadStatus : short { Success = 0, AlreadyLoaded = 1, Error = 2 };

  LoadStatus loadLib(const std::string& libname, const std::string& directory);

  static std::vector<std::tuple<std::string, std::string>> getLibNames();

  std::map<std::string, void*> m_loadedLibs{};

public:
  static ArrowConverterLibraryLoader& instance() {
    static ArrowConverterLibraryLoader me;
    return me;
  }
};

} // namespace podio

#endif // PODIO_ARROWCONVERTERREGISTRY_H
