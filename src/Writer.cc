#include "podio/Writer.h"

#include "podio/ROOTFrameWriter.h"
#ifdef PODIO_ENABLE_RNTUPLE
  #include "podio/RNTupleWriter.h"
#endif
#ifdef PODIO_ENABLE_SIO
  #include "podio/SIOFrameWriter.h"
#endif

#include <memory>

namespace podio {

template <typename T>
Writer::Writer(std::unique_ptr<T> reader) : m_self(std::make_unique<WriterModel<T>>(reader.release())) {
}

std::unique_ptr<Writer> makeWriter(const std::string& filename, const std::string& type) {

  auto endsWith = [](const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
  };

  std::unique_ptr<Writer> writer;
  if ((type == "default" && endsWith(filename, ".root")) || type == "root") {
    std::cout << "Calling makeWriter (root)" << std::endl;
    auto actualWriter = std::make_unique<ROOTFrameWriter>(filename);
    writer = std::make_unique<Writer>(std::move(actualWriter));
  } else if (type == "rntuple") {
#ifdef PODIO_ENABLE_RNTUPLE
    std::cout << "Calling makeWriter (rntuple)" << std::endl;
    auto actualWriter = std::make_unique<RNTupleWriter>(filename);
    writer = std::make_unique<Writer>(std::move(actualWriter));
#else
    std::cout << "ERROR: RNTuple writer not enabled" << std::endl;
#endif
  } else if ((type == "default" && endsWith(filename, ".sio")) || type == "sio") {
#ifdef PODIO_ENABLE_SIO
    std::cout << "Calling makeWriter (sio)" << std::endl;
    auto actualWriter = std::make_unique<SIOFrameWriter>(filename);
    writer = std::make_unique<Writer>(std::move(actualWriter));
#else
    std::cout << "ERROR: SIO writer not enabled" << std::endl;
#endif
  } else {
    std::cout << "ERROR: Unknown writer type " << type << std::endl;
  }
  return writer;
}

} // namespace podio
