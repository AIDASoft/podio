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
#include <any>
#include <cctype>
#include <cstdlib>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

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

  /// Make sure that support for the backend has been built
  ///
  /// @returns The backend for convenient chaining
  /// @throws std::runtime_error if the backend is not available
  Backend ensureAvailable(Backend backend) {
    [[maybe_unused]] auto notAvailable = [backend](std::string_view requirement) {
      return std::runtime_error("Writer type '" + std::string(backendName(backend)) +
                                "' is not available. Please recompile with " + std::string(requirement) + " support.");
    };
    switch (backend) {
    case Backend::TTree:
      break;
    case Backend::RNTuple:
#if !PODIO_ENABLE_RNTUPLE
      throw notAvailable("ROOT RNTuple");
#endif
      break;
    case Backend::SIO:
#if !PODIO_ENABLE_SIO
      throw notAvailable("SIO");
#endif
      break;
    case Backend::Arrow:
#if !(PODIO_ENABLE_ARROW && PODIO_ENABLE_PARQUET)
      throw notAvailable("Arrow and Parquet");
#endif
      break;
    }
    return backend;
  }

  /// Get the options for a backend that takes options of type OptionsT
  ///
  /// @returns A pointer to the options, or a nullptr if no options have been set
  /// @throws std::invalid_argument if the options are not of type OptionsT
  template <typename OptionsT>
  const OptionsT* getOptions(Backend backend, const std::any& options) {
    if (!options.has_value()) {
      return nullptr;
    }
    if (const auto* opts = std::any_cast<OptionsT>(&options)) {
      return opts;
    }
    throw std::invalid_argument("The provided writer options cannot be used for writer type '" +
                                std::string(backendName(backend)) + "'");
  }

  /// Create a low level writer, passing on the options if there are any
  template <typename WriterT, typename OptionsT>
  Writer makeWriter(const std::string& filename, const OptionsT* options) {
    if (options) {
      return Writer{std::make_unique<WriterT>(filename, *options)};
    }
    return Writer{std::make_unique<WriterT>(filename)};
  }

  /// Create the writer for the backend with the (optional) options
  ///
  /// @note The backend has to be available, see ensureAvailable()
  Writer makeWriter(Backend backend, const std::string& filename, const std::any& options) {
    switch (backend) {
    case Backend::TTree:
      return makeWriter<ROOTWriter>(filename, getOptions<ROOTWriteOptions>(backend, options));
    case Backend::RNTuple:
#if PODIO_ENABLE_RNTUPLE
      return makeWriter<RNTupleWriter>(filename, getOptions<ROOTWriteOptions>(backend, options));
#else
      break;
#endif
    case Backend::SIO:
#if PODIO_ENABLE_SIO
      if (options.has_value()) {
        throw std::invalid_argument("The SIOWriter does not provide support for options yet");
      }
      return Writer{std::make_unique<SIOWriter>(filename)};
#else
      break;
#endif
    case Backend::Arrow:
#if PODIO_ENABLE_ARROW && PODIO_ENABLE_PARQUET
      return makeWriter<ArrowWriter>(filename, getOptions<ArrowWriter::Options>(backend, options));
#else
      break;
#endif
    }
    throw std::logic_error("Unhandled or unavailable writer backend '" + std::string(backendName(backend)) + "'");
  }
} // namespace

void WriterFor::ensureNoBackendSelected() const {
  if (m_backend) {
    throw std::invalid_argument("WriterFor: backend has already been set to '" + std::string(backendName(*m_backend)) +
                                "'");
  }
}

WriterFor& WriterFor::withBackend(Backend backend) {
  ensureNoBackendSelected();
  m_backend = ensureAvailable(backend);
  return *this;
}

WriterFor& WriterFor::withType(const std::string& type) {
  ensureNoBackendSelected();
  if (const auto backend = backendFromType(type)) {
    m_backend = ensureAvailable(*backend);
  }
  return *this;
}

Writer WriterFor::create() const {
  const auto backend = m_backend ? *m_backend : ensureAvailable(backendFromFilename(m_filename));
  return makeWriter(backend, m_filename, m_options);
}

Writer makeWriter(const std::string& filename, const std::string& type) {
  return WriterFor{filename}.withType(type).create();
}

} // namespace podio
