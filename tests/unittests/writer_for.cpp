#include "podio/Frame.h"
#include "podio/ROOTWriteOptions.h"
#include "podio/Reader.h"
#include "podio/Writer.h"

#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "catch2/generators/catch_generators_range.hpp"

#if PODIO_ENABLE_ROOT
  #include "Compression.h"
  #include "TFile.h"
  #include "TKey.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

namespace {
using Backend = podio::WriterFor::Backend;

/// Set (or unset) the PODIO_DEFAULT_WRITE_RNTUPLE environment variable for the
/// lifetime of this object
class DefaultRNTupleEnv {
public:
  explicit DefaultRNTupleEnv(bool enable) {
    if (const auto* current = std::getenv(name)) {
      m_previous = current;
    }
    if (enable) {
      setenv(name, "1", 1);
    } else {
      unsetenv(name);
    }
  }
  DefaultRNTupleEnv(const DefaultRNTupleEnv&) = delete;
  DefaultRNTupleEnv& operator=(const DefaultRNTupleEnv&) = delete;
  DefaultRNTupleEnv(DefaultRNTupleEnv&&) = delete;
  DefaultRNTupleEnv& operator=(DefaultRNTupleEnv&&) = delete;
  ~DefaultRNTupleEnv() {
    if (m_previous) {
      setenv(name, m_previous->c_str(), 1);
    } else {
      unsetenv(name);
    }
  }

private:
  static constexpr const char* name = "PODIO_DEFAULT_WRITE_RNTUPLE";
  std::optional<std::string> m_previous{};
};

/// A uniquely named file in the current working directory that is removed (if
/// it exists) when this object goes out of scope
class TemporaryFile {
public:
  explicit TemporaryFile(const std::string& extension) {
    std::random_device rd;
    std::uniform_int_distribution<std::uint64_t> dist{};
    m_name = "podio_unittest_writer_for_" + std::to_string(dist(rd)) + extension;
  }
  TemporaryFile(const TemporaryFile&) = delete;
  TemporaryFile& operator=(const TemporaryFile&) = delete;
  TemporaryFile(TemporaryFile&&) = delete;
  TemporaryFile& operator=(TemporaryFile&&) = delete;
  ~TemporaryFile() {
    std::error_code ec;
    std::filesystem::remove_all(m_name, ec);
  }

  const std::string& name() const {
    return m_name;
  }

private:
  std::string m_name{};
};

/// A writer backend that can be selected without ambiguity via its type name
/// or the extension of the output file
struct BackendInfo {
  Backend backend;
  std::string type;
  std::string extension;
};

/// Get all backends that are available in this build. The (default) TTree
/// backend comes first if it is available.
///
/// @note RNTuple is not part of this as it can not be selected via the
/// extension of the output file alone
std::vector<BackendInfo> availableBackends() {
  return {
#if PODIO_ENABLE_ROOT
      {Backend::TTree, "root", ".root"},
#endif
#if PODIO_ENABLE_SIO
      {Backend::SIO, "sio", ".sio"},
#endif
#if PODIO_ENABLE_ARROW && PODIO_ENABLE_PARQUET
      {Backend::Arrow, "parquet", ".podio_parquet"}
#endif
  };
}

/// Write one (almost empty) event with the passed writer
void writeTestEvent(podio::Writer writer) {
  podio::Frame frame;
  frame.putParameter("value", 42);
  writer.writeEvent(frame);
  writer.finish();
}

/// Read back the first event from a file and check that it contains the value
/// written by writeTestEvent
void checkRoundtrip(const std::string& filename) {
  auto reader = podio::makeReader(filename);
  const auto frame = reader.readNextEvent();
  const auto value = frame.getParameter<int>("value");
  REQUIRE(value.has_value());
  REQUIRE(value.value() == 42);
}

#if PODIO_ENABLE_ROOT
enum class StorageType { TTree, RNTuple, Other };

/// Write one (almost empty) event and determine how it has been stored
StorageType writeAndGetStorage(podio::Writer writer, const std::string& filename) {
  writeTestEvent(std::move(writer));

  TFile file(filename.c_str());
  const auto* key = file.GetKey("events");
  REQUIRE(key);
  const std::string className = key->GetClassName();
  if (className == "TTree") {
    return StorageType::TTree;
  }
  if (className.find("RNTuple") != std::string::npos) {
    return StorageType::RNTuple;
  }
  return StorageType::Other;
}

/// Get the compression settings of a ROOT file
int getCompressionSettings(const std::string& filename) {
  TFile file(filename.c_str());
  return file.GetCompressionSettings();
}
#endif
} // namespace

#if PODIO_ENABLE_ROOT
TEST_CASE("WriterFor backend selection", "[writer-for]") {
  const TemporaryFile file{".root"};
  const auto& filename = file.name();

  SECTION("TTree by default") {
    DefaultRNTupleEnv env{false};
    REQUIRE(writeAndGetStorage(podio::WriterFor{filename}.create(), filename) == StorageType::TTree);
    REQUIRE(writeAndGetStorage(podio::makeWriter(filename), filename) == StorageType::TTree);

    REQUIRE(writeAndGetStorage(podio::WriterFor{filename}.withType("DEFAULT").create(), filename) ==
            StorageType::TTree);
  }

  SECTION("Explicit backend takes precedence over environment") {
    DefaultRNTupleEnv env{true};
    REQUIRE(writeAndGetStorage(podio::WriterFor{filename}.withType("ROOT").create(), filename) == StorageType::TTree);
    REQUIRE(writeAndGetStorage(podio::makeWriter(filename, "root"), filename) == StorageType::TTree);

    REQUIRE(writeAndGetStorage(podio::WriterFor{filename}.withBackend(Backend::TTree).create(), filename) ==
            StorageType::TTree);
  }

  SECTION("ROOT options follow the default backend") {
    DefaultRNTupleEnv env{false};
    REQUIRE(writeAndGetStorage(
                podio::WriterFor{filename}.withOptions(podio::ROOTWriteOptions{.compressionSettings = 0}).create(),
                filename) == StorageType::TTree);
    REQUIRE(getCompressionSettings(filename) == 0);
    checkRoundtrip(filename);
  }

  SECTION("ROOT compression options are applied") {
    DefaultRNTupleEnv env{false};
    constexpr int kUseAnalysis = ROOT::RCompressionSetting::EDefaults::kUseAnalysis;
    REQUIRE(writeAndGetStorage(podio::WriterFor{filename}
                                   .withOptions(podio::ROOTWriteOptions{.compressionSettings = kUseAnalysis})
                                   .create(),
                               filename) == StorageType::TTree);
    REQUIRE(getCompressionSettings(filename) == kUseAnalysis);
    checkRoundtrip(filename);
  }

  #if PODIO_ENABLE_RNTUPLE
  SECTION("Environment switches to RNTuple") {
    DefaultRNTupleEnv env{true};
    REQUIRE(writeAndGetStorage(podio::WriterFor{filename}.create(), filename) == StorageType::RNTuple);
    REQUIRE(writeAndGetStorage(podio::makeWriter(filename), filename) == StorageType::RNTuple);
  }

  SECTION("Explicit backend selects RNTuple") {
    DefaultRNTupleEnv env{false};
    REQUIRE(writeAndGetStorage(podio::WriterFor{filename}.withBackend(Backend::RNTuple).create(), filename) ==
            StorageType::RNTuple);
    REQUIRE(writeAndGetStorage(podio::WriterFor{filename}.withType("RNTuple").create(), filename) ==
            StorageType::RNTuple);
  }
  #endif
}
#endif

TEST_CASE("WriterFor roundtrip for all available backends", "[writer-for]") {
  const auto backends = availableBackends();
  if (backends.empty()) {
    SKIP("No writer backend available in this build");
  }
  const auto info = GENERATE_COPY(from_range(backends));
  CAPTURE(info.type);

  DefaultRNTupleEnv env{false};
  const TemporaryFile file{info.extension};
  const auto& filename = file.name();

  SECTION("Backend from file extension") {
    writeTestEvent(podio::WriterFor{filename}.create());
  }

  SECTION("Explicit type") {
    writeTestEvent(podio::WriterFor{filename}.withType(info.type).create());
  }

  SECTION("Explicit backend") {
    writeTestEvent(podio::WriterFor{filename}.withBackend(info.backend).create());
  }

  SECTION("makeWriter") {
    writeTestEvent(podio::makeWriter(filename));
  }

  if (info.backend != Backend::TTree) {
    SECTION("Environment has no effect on non ROOT files") {
      DefaultRNTupleEnv rntupleEnv{true};
      writeTestEvent(podio::WriterFor{filename}.create());
    }
  }

  checkRoundtrip(filename);
}

TEST_CASE("WriterFor configuration errors", "[writer-for]") {
  const TemporaryFile file{".root"};
  const auto& filename = file.name();

#if !PODIO_ENABLE_ROOT
  SECTION("ROOT not available") {
    REQUIRE_THROWS_AS(podio::WriterFor{filename}.withType("root"), std::runtime_error);
    REQUIRE_THROWS_AS(podio::WriterFor{filename}.withBackend(Backend::TTree), std::runtime_error);
    REQUIRE_THROWS_AS(podio::makeWriter(filename, "root"), std::runtime_error);
    {
      DefaultRNTupleEnv env{false};
      REQUIRE_THROWS_AS(podio::WriterFor{filename}.create(), std::runtime_error);
      REQUIRE_THROWS_AS(podio::makeWriter(filename), std::runtime_error);
      // Availability is checked before the options
      REQUIRE_THROWS_AS(podio::WriterFor{filename}.withOptions(podio::ROOTWriteOptions{}).create(), std::runtime_error);
    }
    {
      DefaultRNTupleEnv env{true};
      REQUIRE_THROWS_AS(podio::WriterFor{filename}.create(), std::runtime_error);
    }
    REQUIRE_FALSE(std::filesystem::exists(filename));
  }
#endif

#if !PODIO_ENABLE_RNTUPLE
  SECTION("RNTuple not available") {
    REQUIRE_THROWS_AS(podio::WriterFor{filename}.withType("rntuple"), std::runtime_error);
    REQUIRE_THROWS_AS(podio::WriterFor{filename}.withBackend(Backend::RNTuple), std::runtime_error);
  }
#endif

#if PODIO_ENABLE_SIO
  SECTION("Options not matching the explicit type") {
    auto builder = podio::WriterFor{filename}.withType("sio").withOptions(podio::ROOTWriteOptions{});
    REQUIRE_THROWS_AS(builder.create(), std::invalid_argument);
    REQUIRE_FALSE(std::filesystem::exists(filename));
  }

  SECTION("ROOT options for a non ROOT file") {
    const TemporaryFile sioFile{".sio"};
    auto builder = podio::WriterFor{sioFile.name()}.withOptions(podio::ROOTWriteOptions{});
    REQUIRE_THROWS_AS(builder.create(), std::invalid_argument);
    REQUIRE_FALSE(std::filesystem::exists(sioFile.name()));
  }
#else
  SECTION("SIO not available") {
    REQUIRE_THROWS_AS(podio::WriterFor{filename}.withType("sio"), std::runtime_error);
    REQUIRE_THROWS_AS(podio::WriterFor{filename}.withBackend(Backend::SIO), std::runtime_error);
    REQUIRE_THROWS_AS(podio::makeWriter(filename, "sio"), std::runtime_error);
    const TemporaryFile sioFile{".sio"};
    REQUIRE_THROWS_AS(podio::WriterFor{sioFile.name()}.create(), std::runtime_error);
    REQUIRE_FALSE(std::filesystem::exists(sioFile.name()));
  }
#endif

#if PODIO_ENABLE_ARROW && PODIO_ENABLE_PARQUET
  SECTION("Options not matching the explicit backend") {
    auto builder = podio::WriterFor{filename}.withBackend(Backend::Arrow).withOptions(podio::ROOTWriteOptions{});
    REQUIRE_THROWS_AS(builder.create(), std::invalid_argument);
    REQUIRE_FALSE(std::filesystem::exists(filename));
  }
#else
  SECTION("Arrow not available") {
    REQUIRE_THROWS_AS(podio::WriterFor{filename}.withType("parquet"), std::runtime_error);
    REQUIRE_THROWS_AS(podio::WriterFor{filename}.withBackend(Backend::Arrow), std::runtime_error);
    const TemporaryFile arrowFile{".podio_parquet"};
    REQUIRE_THROWS_AS(podio::WriterFor{arrowFile.name()}.create(), std::runtime_error);
    REQUIRE_FALSE(std::filesystem::exists(arrowFile.name()));
  }
#endif

  SECTION("Backend can only be selected once") {
    const auto backends = availableBackends();
    if (backends.empty()) {
      SKIP("No writer backend available in this build");
    }
    // TTree if ROOT is available
    const auto& info = backends.front();
    const TemporaryFile backendFile{info.extension};

    auto builder = podio::WriterFor{backendFile.name()}.withBackend(info.backend);
    // The check for an already selected backend comes before the check for
    // availability, so RNTuple works for this check even if it is not available
    REQUIRE_THROWS_AS(builder.withBackend(Backend::RNTuple), std::invalid_argument);
    REQUIRE_THROWS_AS(builder.withType(info.type), std::invalid_argument);
    REQUIRE_THROWS_AS(builder.withType("default"), std::invalid_argument);

    auto otherBuilder = podio::WriterFor{backendFile.name()}.withType(info.type);
    REQUIRE_THROWS_AS(otherBuilder.withBackend(info.backend), std::invalid_argument);
    REQUIRE_THROWS_AS(otherBuilder.withType("rntuple"), std::invalid_argument);

    REQUIRE_FALSE(std::filesystem::exists(backendFile.name()));
  }

  SECTION("Options of an unrelated type") {
    const auto backends = availableBackends();
    if (backends.empty()) {
      SKIP("No writer backend available in this build");
    }
    // TTree if ROOT is available
    const auto& info = backends.front();
    const TemporaryFile backendFile{info.extension};

    struct NotWriterOptions {
      int value{0};
    };
    DefaultRNTupleEnv env{false};
    auto builder = podio::WriterFor{backendFile.name()}.withOptions(NotWriterOptions{});
    REQUIRE_THROWS_AS(builder.create(), std::invalid_argument);
    REQUIRE_THROWS_AS(podio::WriterFor{backendFile.name()}.withType(info.type).withOptions(NotWriterOptions{}).create(),
                      std::invalid_argument);
    REQUIRE_FALSE(std::filesystem::exists(backendFile.name()));
  }

  SECTION("Unknown type") {
    auto builder = podio::WriterFor{filename};
    REQUIRE_THROWS_AS(builder.withType("not-a-backend"), std::runtime_error);
    REQUIRE_THROWS_AS(podio::makeWriter(filename, "not-a-backend"), std::runtime_error);
    REQUIRE_FALSE(std::filesystem::exists(filename));
  }

  SECTION("Unknown file extension") {
    const TemporaryFile unknownFile{".unknown"};
    REQUIRE_THROWS_AS(podio::WriterFor{unknownFile.name()}.create(), std::runtime_error);
    REQUIRE_FALSE(std::filesystem::exists(unknownFile.name()));
  }
}
