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
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <typeindex>

namespace podio {

namespace {
  using Backend = WriterFor::Backend;

  constexpr std::string_view backendName(Backend backend) {
    switch (backend) {
    case Backend::TTree:
      return "root";
    case Backend::RNTuple:
      return "rntuple";
    case Backend::SIO:
      return "sio";
    case Backend::Arrow:
      return "parquet";
    }
    return "unknown";
  }

  /// Get the explicitly requested backend (if any) from the type
  std::optional<Backend> backendFromType(const std::string& type) {
    auto lower = [](std::string str) {
      std::ranges::transform(str, str.begin(), [](unsigned char c) { return std::tolower(c); });
      return str;
    };
    const auto lowerType = lower(type);
    if (lowerType == "default") {
      return std::nullopt;
    }
    for (const auto backend : {Backend::TTree, Backend::RNTuple, Backend::SIO, Backend::Arrow}) {
      if (lowerType == backendName(backend)) {
        return backend;
      }
    }
    throw std::runtime_error("Unknown writer type '" + type + "'");
  }

  /// Determine the backend from the filename (and the environment)
  Backend backendFromFilename(const std::string& filename) {
    auto endsWith = [](const std::string& str, const std::string& suffix) {
      return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
    };

    if (endsWith(filename, ".root")) {
      const char* defaultRNTupleEnv = std::getenv("PODIO_DEFAULT_WRITE_RNTUPLE");
      const bool defaultRNTuple = defaultRNTupleEnv && *defaultRNTupleEnv != '\0';
      return defaultRNTuple ? Backend::RNTuple : Backend::TTree;
    }
    if (endsWith(filename, ".sio")) {
      return Backend::SIO;
    }
    if (endsWith(filename, ".podio_parquet")) {
      return Backend::Arrow;
    }
    throw std::runtime_error("Unknown file type for file " + filename);
  }

  /// Check whether the low level writer type belongs to the backend
  bool isWriterFor(Backend backend, std::type_index writerType) {
    switch (backend) {
    case Backend::TTree:
      return writerType == typeid(ROOTWriter);
    case Backend::RNTuple:
#if PODIO_ENABLE_RNTUPLE
      return writerType == typeid(RNTupleWriter);
#else
      return false;
#endif
    case Backend::SIO:
#if PODIO_ENABLE_SIO
      return writerType == typeid(SIOWriter);
#else
      return false;
#endif
    case Backend::Arrow:
#if PODIO_ENABLE_ARROW && PODIO_ENABLE_PARQUET
      return writerType == typeid(ArrowWriter);
#else
      return false;
#endif
    }
    return false;
  }

  /// Create a writer with default options for the given backend
  Writer makeDefaultWriter(Backend backend, const std::string& filename) {
    switch (backend) {
    case Backend::TTree:
      return Writer{std::make_unique<ROOTWriter>(filename)};
    case Backend::RNTuple:
#if PODIO_ENABLE_RNTUPLE
      return Writer{std::make_unique<RNTupleWriter>(filename)};
#else
      throw std::runtime_error("ROOT RNTuple writer not available. Please recompile with ROOT RNTuple support.");
#endif
    case Backend::SIO:
#if PODIO_ENABLE_SIO
      return Writer{std::make_unique<SIOWriter>(filename)};
#else
      throw std::runtime_error("SIO writer not available. Please recompile with SIO support.");
#endif
    case Backend::Arrow:
#if PODIO_ENABLE_ARROW && PODIO_ENABLE_PARQUET
      return Writer{std::make_unique<ArrowWriter>(filename)};
#else
      throw std::runtime_error("Arrow writer not available. Please recompile with Arrow and Parquet support.");
#endif
    }
    throw std::logic_error("Unhandled writer backend");
  }
} // namespace

WriterFor& WriterFor::withBackend(Backend backend) {
  if (m_backendSelection == BackendSelection::ViaType) {
    throw std::invalid_argument("WriterFor: withBackend() cannot be combined with withType()");
  }
  m_backendSelection = BackendSelection::ViaBackend;
  m_backend = backend;
  return *this;
}

WriterFor& WriterFor::withType(const std::string& type) {
  if (m_backendSelection == BackendSelection::ViaBackend) {
    throw std::invalid_argument("WriterFor: withType() cannot be combined with withBackend()");
  }
  m_backendSelection = BackendSelection::ViaType;
  m_backend = backendFromType(type);
  return *this;
}

Writer WriterFor::create() const {
  if (m_makeWriter) {
    if (m_backend && !isWriterFor(*m_backend, *m_writerType)) {
      throw std::invalid_argument("Options of a different writer cannot be used for writer type '" +
                                  std::string(backendName(*m_backend)) + "'");
    }
    return m_makeWriter(m_filename);
  }

  const auto backend = m_backend ? *m_backend : backendFromFilename(m_filename);
  return makeDefaultWriter(backend, m_filename);
}

Writer makeWriter(const std::string& filename, const std::string& type) {
  return WriterFor{filename}.withType(type).create();
}

} // namespace podio
