#include "podio/Writer.h"

#include "podio/ROOTFrameWriter.h"
#if PODIO_ENABLE_RNTUPLE
  #include "podio/RNTupleWriter.h"
#endif
#if PODIO_ENABLE_SIO
  #include "podio/SIOFrameWriter.h"
#endif

#include <memory>

namespace podio {

template <typename T>
Writer::Writer(std::unique_ptr<T> reader) : m_self(std::make_unique<WriterModel<T>>(reader.release())) {
}

Writer makeWriter(const std::string& filename, const std::string& type) {

  auto endsWith = [](const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
  };

  auto lower = [](std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
    return str;
  };

  if ((type == "default" && endsWith(filename, ".root")) || lower(type) == "root") {
    auto actualWriter = std::make_unique<ROOTFrameWriter>(filename);
    Writer writer{std::move(actualWriter)};
    return writer;
  }
  if (lower(type) == "rntuple") {
#if PODIO_ENABLE_RNTUPLE
    auto actualWriter = std::make_unique<RNTupleWriter>(filename);
    Writer writer{std::move(actualWriter)};
    return writer;
#else
    throw std::runtime_error("ROOT RNTuple writer not available. Please recompile with ROOT RNTuple support.");
#endif
  }
  if ((type == "default" && endsWith(filename, ".sio")) || lower(type) == "sio") {
#if PODIO_ENABLE_SIO
    auto actualWriter = std::make_unique<SIOFrameWriter>(filename);
    Writer writer{std::move(actualWriter)};
    return writer;
#else
    throw std::runtime_error("SIO writer not available. Please recompile with SIO support.");
#endif
  }
  throw std::runtime_error("Unknown file type");
}

} // namespace podio
