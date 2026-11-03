#include "podio/Frame.h"
#include "podio/ROOTWriter.h"
#include "podio/Writer.h"
#if PODIO_ENABLE_RNTUPLE
  #include "podio/RNTupleWriter.h"
#endif

#include "catch2/catch_test_macros.hpp"

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

  SECTION("Default type resets explicit backend") {
    DefaultRNTupleEnv env{false};
    REQUIRE(writeAndGetStorage(podio::WriterFor{filename}.withType("sio").withType("default").create(), filename) ==
            StorageType::TTree);
  }

  SECTION("Options take precedence over environment") {
    DefaultRNTupleEnv env{true};
    REQUIRE(writeAndGetStorage(podio::WriterFor{filename}.withOptions(podio::ROOTWriter::Options{}).create(),
                               filename) == StorageType::TTree);
  }

#if PODIO_ENABLE_RNTUPLE
  SECTION("Environment switches to RNTuple") {
    DefaultRNTupleEnv env{true};
    REQUIRE(writeAndGetStorage(podio::WriterFor{filename}.create(), filename) == StorageType::RNTuple);
    REQUIRE(writeAndGetStorage(podio::makeWriter(filename), filename) == StorageType::RNTuple);
  }

  SECTION("Options select RNTuple") {
    DefaultRNTupleEnv env{false};
    REQUIRE(writeAndGetStorage(podio::WriterFor{filename}.withOptions(podio::RNTupleWriter::Options{}).create(),
                               filename) == StorageType::RNTuple);
  }

  SECTION("Explicit backend selects RNTuple") {
    DefaultRNTupleEnv env{false};
    REQUIRE(writeAndGetStorage(podio::WriterFor{filename}.withBackend(podio::WriterFor::Backend::RNTuple).create(),
                               filename) == StorageType::RNTuple);
  }

  SECTION("Explicit type and matching options") {
    DefaultRNTupleEnv env{false};
    REQUIRE(writeAndGetStorage(
                podio::WriterFor{filename}.withType("rntuple").withOptions(podio::RNTupleWriter::Options{}).create(),
                filename) == StorageType::RNTuple);
  }
#endif
}

TEST_CASE("WriterFor configuration errors", "[writer-for]") {
  const TemporaryFile file{".root"};
  const auto& filename = file.name();

  SECTION("Options not matching the explicit type") {
    auto builder = podio::WriterFor{filename}.withType("sio").withOptions(podio::ROOTWriter::Options{});
    REQUIRE_THROWS_AS(builder.create(), std::invalid_argument);
    REQUIRE_FALSE(std::filesystem::exists(filename));
  }

  SECTION("Options not matching the explicit backend") {
    auto builder = podio::WriterFor{filename}
                       .withBackend(podio::WriterFor::Backend::Arrow)
                       .withOptions(podio::ROOTWriter::Options{});
    REQUIRE_THROWS_AS(builder.create(), std::invalid_argument);
    REQUIRE_FALSE(std::filesystem::exists(filename));
  }

  SECTION("withBackend and withType cannot be combined") {
    auto builder = podio::WriterFor{filename}.withBackend(podio::WriterFor::Backend::TTree);
    REQUIRE_THROWS_AS(builder.withType("root"), std::invalid_argument);
    REQUIRE_THROWS_AS(builder.withType("default"), std::invalid_argument);

    auto otherBuilder = podio::WriterFor{filename}.withType("root");
    REQUIRE_THROWS_AS(otherBuilder.withBackend(podio::WriterFor::Backend::TTree), std::invalid_argument);
    REQUIRE_NOTHROW(otherBuilder.withType("rntuple"));

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
