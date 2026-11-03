#include "podio/Frame.h"
#include "podio/ROOTReader.h"
#include "podio/ROOTWriter.h"
#include "podio/Writer.h"
#if PODIO_ENABLE_RNTUPLE
  #include "podio/RNTupleWriter.h"
#endif

#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"

#include "Compression.h"
#include "TFile.h"
#include "TKey.h"

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <system_error>

namespace {
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

enum class StorageType { TTree, RNTuple, Other };

/// Write one (almost empty) event and determine how it has been stored
StorageType writeAndGetStorage(podio::Writer writer, const std::string& filename) {
  podio::Frame frame;
  frame.putParameter("value", 42);
  writer.writeEvent(frame);
  writer.finish();

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

/// Read back the first event from a (TTree based) ROOT file and check that it
/// contains the value written by writeAndGetStorage
void checkRoundtrip(const std::string& filename) {
  podio::ROOTReader reader;
  reader.openFile(filename);
  const auto frame = podio::Frame(reader.readNextEntry("events"));
  const auto value = frame.getParameter<int>("value");
  REQUIRE(value.has_value());
  REQUIRE(value.value() == 42);
}
} // namespace

TEST_CASE("WriterFor backend selection", "[writer-for]") {
  const TemporaryFile file{".root"};
  const auto& filename = file.name();

  SECTION("TTree by default") {
    DefaultRNTupleEnv env{false};
    REQUIRE(writeAndGetStorage(podio::WriterFor{filename}.create(), filename) == StorageType::TTree);
    REQUIRE(writeAndGetStorage(podio::makeWriter(filename), filename) == StorageType::TTree);
  }

  SECTION("Explicit type takes precedence over environment") {
    DefaultRNTupleEnv env{true};
    REQUIRE(writeAndGetStorage(podio::WriterFor{filename}.withType("ROOT").create(), filename) == StorageType::TTree);
    REQUIRE(writeAndGetStorage(podio::makeWriter(filename, "root"), filename) == StorageType::TTree);
  }

  SECTION("Explicit backend takes precedence over environment") {
    DefaultRNTupleEnv env{true};
    REQUIRE(writeAndGetStorage(podio::WriterFor{filename}.withBackend(podio::WriterFor::Backend::TTree).create(),
                               filename) == StorageType::TTree);
  }

  SECTION("Default type does not select a backend") {
    DefaultRNTupleEnv env{false};
    REQUIRE(writeAndGetStorage(podio::WriterFor{filename}.withType("default").create(), filename) ==
            StorageType::TTree);
    REQUIRE(writeAndGetStorage(
                podio::WriterFor{filename}.withType("default").withBackend(podio::WriterFor::Backend::TTree).create(),
                filename) == StorageType::TTree);
  }

  SECTION("ROOT options follow the default backend") {
    DefaultRNTupleEnv env{false};
    REQUIRE(writeAndGetStorage(podio::WriterFor{filename}.withOptions({.compressionSettings = 0}).create(), filename) ==
            StorageType::TTree);
    REQUIRE(getCompressionSettings(filename) == 0);
    checkRoundtrip(filename);
  }

  SECTION("ROOT options with explicit TTree backend") {
    DefaultRNTupleEnv env{true};
    constexpr int kUseAnalysis = ROOT::RCompressionSetting::EDefaults::kUseAnalysis;
    REQUIRE(writeAndGetStorage(podio::WriterFor{filename}
                                   .withBackend(podio::WriterFor::Backend::TTree)
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

  SECTION("ROOT options follow the environment") {
    DefaultRNTupleEnv env{true};
    REQUIRE(writeAndGetStorage(podio::WriterFor{filename}.withOptions(podio::ROOTWriteOptions{}).create(), filename) ==
            StorageType::RNTuple);
  }

  SECTION("Explicit backend selects RNTuple") {
    DefaultRNTupleEnv env{false};
    REQUIRE(writeAndGetStorage(podio::WriterFor{filename}.withBackend(podio::WriterFor::Backend::RNTuple).create(),
                               filename) == StorageType::RNTuple);
  }

  SECTION("Explicit type and matching options") {
    DefaultRNTupleEnv env{false};
    REQUIRE(writeAndGetStorage(
                podio::WriterFor{filename}.withType("rntuple").withOptions(podio::ROOTWriteOptions{}).create(),
                filename) == StorageType::RNTuple);
  }
#endif
}

TEST_CASE("ROOTWriter compression settings", "[writer-for][root-write-options]") {
  using EDefaults = ROOT::RCompressionSetting::EDefaults;
  const TemporaryFile file{".root"};
  const auto& filename = file.name();

  SECTION("Default options use the compiled in default") {
    {
      podio::ROOTWriter writer(filename);
      podio::Frame frame;
      frame.putParameter("value", 42);
      writer.writeFrame(frame, "events");
    }
    REQUIRE(getCompressionSettings(filename) == EDefaults::kUseCompiledDefault);
    checkRoundtrip(filename);
  }

  SECTION("Explicit compression settings are applied") {
    // 0 explicitly disables compression
    const int compression =
        GENERATE(EDefaults::kUseGeneralPurpose, EDefaults::kUseSmallest, EDefaults::kUseAnalysis, 0);
    CAPTURE(compression);
    {
      podio::ROOTWriter writer(filename, podio::ROOTWriteOptions{.compressionSettings = compression});
      podio::Frame frame;
      frame.putParameter("value", 42);
      writer.writeFrame(frame, "events");
    }
    REQUIRE(getCompressionSettings(filename) == compression);
    checkRoundtrip(filename);
  }
}

TEST_CASE("WriterFor configuration errors", "[writer-for]") {
  const TemporaryFile file{".root"};
  const auto& filename = file.name();

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
    REQUIRE_THROWS_AS(podio::WriterFor{filename}.withBackend(podio::WriterFor::Backend::SIO), std::runtime_error);
    REQUIRE_THROWS_AS(podio::makeWriter(filename, "sio"), std::runtime_error);
    const TemporaryFile sioFile{".sio"};
    REQUIRE_THROWS_AS(podio::WriterFor{sioFile.name()}.create(), std::runtime_error);
    REQUIRE_FALSE(std::filesystem::exists(sioFile.name()));
  }
#endif

#if PODIO_ENABLE_ARROW && PODIO_ENABLE_PARQUET
  SECTION("Options not matching the explicit backend") {
    auto builder =
        podio::WriterFor{filename}.withBackend(podio::WriterFor::Backend::Arrow).withOptions(podio::ROOTWriteOptions{});
    REQUIRE_THROWS_AS(builder.create(), std::invalid_argument);
    REQUIRE_FALSE(std::filesystem::exists(filename));
  }
#else
  SECTION("Arrow not available") {
    REQUIRE_THROWS_AS(podio::WriterFor{filename}.withType("parquet"), std::runtime_error);
    REQUIRE_THROWS_AS(podio::WriterFor{filename}.withBackend(podio::WriterFor::Backend::Arrow), std::runtime_error);
    const TemporaryFile arrowFile{".podio_parquet"};
    REQUIRE_THROWS_AS(podio::WriterFor{arrowFile.name()}.create(), std::runtime_error);
    REQUIRE_FALSE(std::filesystem::exists(arrowFile.name()));
  }
#endif

#if !PODIO_ENABLE_RNTUPLE
  SECTION("RNTuple not available") {
    REQUIRE_THROWS_AS(podio::WriterFor{filename}.withType("rntuple"), std::runtime_error);
    REQUIRE_THROWS_AS(podio::WriterFor{filename}.withBackend(podio::WriterFor::Backend::RNTuple), std::runtime_error);
  }
#endif

  SECTION("Backend can only be selected once") {
    auto builder = podio::WriterFor{filename}.withBackend(podio::WriterFor::Backend::TTree);
    REQUIRE_THROWS_AS(builder.withBackend(podio::WriterFor::Backend::RNTuple), std::invalid_argument);
    REQUIRE_THROWS_AS(builder.withType("root"), std::invalid_argument);
    REQUIRE_THROWS_AS(builder.withType("default"), std::invalid_argument);

    auto otherBuilder = podio::WriterFor{filename}.withType("root");
    REQUIRE_THROWS_AS(otherBuilder.withBackend(podio::WriterFor::Backend::TTree), std::invalid_argument);
    REQUIRE_THROWS_AS(otherBuilder.withType("rntuple"), std::invalid_argument);

    REQUIRE_FALSE(std::filesystem::exists(filename));
  }

  SECTION("Options of an unrelated type") {
    struct NotWriterOptions {
      int value{0};
    };
    auto builder = podio::WriterFor{filename}.withOptions(NotWriterOptions{});
    REQUIRE_THROWS_AS(builder.create(), std::invalid_argument);
    REQUIRE_THROWS_AS(podio::WriterFor{filename}.withType("root").withOptions(NotWriterOptions{}).create(),
                      std::invalid_argument);
    REQUIRE_FALSE(std::filesystem::exists(filename));
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
