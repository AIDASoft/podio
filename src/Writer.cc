#include "podio/Writer.h"

#include "podio/ROOTWriter.h"
#if PODIO_ENABLE_RNTUPLE
  #include "podio/RNTupleWriter.h"
#endif
#if PODIO_ENABLE_SIO
  #include "podio/SIOWriter.h"
#endif
#if PODIO_ENABLE_ARROW && PODIO_ENABLE_PARQUET
  #include "podio/ArrowWriter.h"
#endif

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <memory>
#include <stdexcept>

namespace podio {

Writer makeWriter(const std::string& filename, const std::string& type) {
  return makeWriter(filename, type, WriterOptions{});
}

Writer makeWriter(const std::string& filename, const std::string& type, const WriterOptions& options) {

  auto endsWith = [](const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
  };

  auto lower = [](std::string str) {
    std::ranges::transform(str, str.begin(), [](unsigned char c) { return std::tolower(c); });
    return str;
  };

  const char* defaultTypeRNTupleEnv = std::getenv("PODIO_DEFAULT_WRITE_RNTUPLE");
  bool defaultTypeRNTuple;
  if (!defaultTypeRNTupleEnv || std::string(defaultTypeRNTupleEnv).empty()) {
    defaultTypeRNTuple = false;
  } else {
    defaultTypeRNTuple = true;
  }

  if ((type == "default" && !defaultTypeRNTuple && endsWith(filename, ".root")) || lower(type) == "root") {
    ROOTWriter::Options rOpts;
    if (options.root.compressionSettings != 0) {
      rOpts.compressionSettings = options.root.compressionSettings;
    }
    return Writer{std::make_unique<ROOTWriter>(filename, rOpts)};
  } else if ((type == "default" && defaultTypeRNTuple && endsWith(filename, ".root")) || lower(type) == "rntuple") {
#if PODIO_ENABLE_RNTUPLE
    RNTupleWriter::Options rOpts;
    if (options.rntuple.compressionSettings != 0) {
      rOpts.compressionSettings = options.rntuple.compressionSettings;
    }
    return Writer{std::make_unique<RNTupleWriter>(filename, rOpts)};
#else
    throw std::runtime_error("ROOT RNTuple writer not available. Please recompile with ROOT RNTuple support.");
#endif
  } else if (endsWith(filename, ".sio")) {
#if PODIO_ENABLE_SIO
    return Writer{std::make_unique<SIOWriter>(filename)};
#else
    throw std::runtime_error("SIO writer not available. Please recompile with SIO support.");
#endif
  } else if (endsWith(filename, ".podio_parquet") || lower(type) == "parquet") {
#if PODIO_ENABLE_ARROW && PODIO_ENABLE_PARQUET
    ArrowWriter::Options aOpts;
    if (!options.arrow.compression.empty()) {
      aOpts.compression = options.arrow.compression;
    }
    if (options.arrow.maxBufferedRows != 0) {
      aOpts.maxBufferedRows = options.arrow.maxBufferedRows;
    }
    return Writer{std::make_unique<ArrowWriter>(filename, aOpts)};
#else
    throw std::runtime_error("Arrow writer not available. Please recompile with Arrow and Parquet support.");
#endif
  }
  throw std::runtime_error("Unknown file type for file " + filename + " with type " + type);
}

} // namespace podio
