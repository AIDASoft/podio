#include "podio/Frame.h"
#include "podio/ROOTReader.h"
#include "podio/ROOTWriter.h"

#include "TFile.h"

#include <cassert>
#include <iostream>
#include <string>

static void verifyFileCompression(const std::string& filename, int expected) {
  TFile f(filename.c_str());
  const int actual = f.GetCompressionSettings();
  if (actual != expected) {
    std::cerr << "FAIL " << filename << ": expected compression " << expected << ", got " << actual << "\n";
    std::exit(1);
  }
}

static void verifyRoundtrip(const std::string& filename) {
  podio::ROOTReader reader;
  reader.openFile(filename);
  auto frame = podio::Frame(reader.readNextEntry("events"));
  const auto val = frame.getParameter<int>("sentinel");
  if (!val || val.value() != 42) {
    std::cerr << "FAIL " << filename << ": round-trip read returned wrong sentinel value\n";
    std::exit(1);
  }
}

static void writeWithOptions(const std::string& filename, const podio::ROOTWriter::Options& opts) {
  podio::ROOTWriter writer(filename, opts);
  podio::Frame frame;
  frame.putParameter("sentinel", 42);
  writer.writeFrame(frame, "events");
}

static void writeDefault(const std::string& filename) {
  podio::ROOTWriter writer(filename);
  podio::Frame frame;
  frame.putParameter("sentinel", 42);
  writer.writeFrame(frame, "events");
}

int main() {
  // Default constructor must produce ZLIB-1 (101)
  writeDefault("compression_default.root");
  verifyFileCompression("compression_default.root", 101);
  verifyRoundtrip("compression_default.root");

  // Options with ZSTD-5 (505)
  writeWithOptions("compression_zstd5.root", {.compressionSettings = 505});
  verifyFileCompression("compression_zstd5.root", 505);
  verifyRoundtrip("compression_zstd5.root");

  // Options with LZMA-9 (209)
  writeWithOptions("compression_lzma9.root", {.compressionSettings = 209});
  verifyFileCompression("compression_lzma9.root", 209);
  verifyRoundtrip("compression_lzma9.root");

  std::cout << "All compression tests passed.\n";
  return 0;
}
