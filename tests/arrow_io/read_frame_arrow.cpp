#include "read_frame.h"
#include "read_frame_auxiliary.h"

#include "podio/ArrowReader.h"

#include <iostream>
#include <stdexcept>
#include <string>

int test_arrow_reader_invalid_coll(const std::string& inputFile) {
  auto reader = podio::ArrowReader();
  reader.openFile(inputFile);
  try {
    reader.readEntry("events", 0, {"non_existent_collection"});
    std::cerr << "Expected std::invalid_argument for non-existent collection" << std::endl;
    return 1;
  } catch (const std::invalid_argument&) {
    return 0;
  }
}

int test_arrow_reader_edge_cases(const std::string& inputFile) {
  auto reader = podio::ArrowReader();
  reader.openFile(inputFile);

  // Edge case 1: duplicate collection names in collsToRead
  auto frameDup = podio::Frame(reader.readEntry("events", 0, {"mcparticles", "mcparticles"}));
  if (frameDup.get("mcparticles") == nullptr) {
    std::cerr << "Failed to read entry with duplicate collection names in collsToRead" << std::endl;
    return 1;
  }

  // Edge case 2: expanding collections across successive reads
  auto frameA = podio::Frame(reader.readEntry("events", 0, {"mcparticles"}));
  if (frameA.get("mcparticles") == nullptr) {
    std::cerr << "Failed to read entry with subset collection" << std::endl;
    return 1;
  }

  auto frameAB = podio::Frame(reader.readEntry("events", 1, {"mcparticles", "clusters"}));
  if (frameAB.get("mcparticles") == nullptr || frameAB.get("clusters") == nullptr) {
    std::cerr << "Failed to read entry with expanded collection set" << std::endl;
    return 1;
  }

  // Edge case 3: reading all collections after subset read
  auto frameAll = podio::Frame(reader.readEntry("events", 2, {}));
  if (frameAll.get("mcparticles") == nullptr || frameAll.get("clusters") == nullptr) {
    std::cerr << "Failed to read full entry after subset read" << std::endl;
    return 1;
  }

  return 0;
}

int main(int argc, char* argv[]) {
  std::string inputFile = "example_frame.podio_parquet";
  bool assertBuildVersion = true;
  if (argc == 2) {
    inputFile = argv[1];
    assertBuildVersion = false;
  }

  return read_frames<podio::ArrowReader>(inputFile, assertBuildVersion) +
      test_frame_aux_info<podio::ArrowReader>(inputFile) + test_read_frame_limited<podio::ArrowReader>(inputFile) +
      test_arrow_reader_invalid_coll(inputFile) + test_arrow_reader_edge_cases(inputFile);
}
