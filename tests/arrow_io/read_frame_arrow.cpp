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

int test_arrow_reader_collection_id_mapping(const std::string& inputFile) {
  // Obtain all collection names and their IDs from a full read
  auto fullReader = podio::ArrowReader();
  fullReader.openFile(inputFile);
  const auto fullFrame = podio::Frame(fullReader.readEntry("events", 0, {}));

  // Fresh reader performing a projected read selecting only {"clusters"}
  auto projReader = podio::ArrowReader();
  projReader.openFile(inputFile);
  const auto projFrame = podio::Frame(projReader.readEntry("events", 0, {"clusters"}));

  // Selected collection payload must be available
  if (projFrame.get("clusters") == nullptr) {
    std::cerr << "Expected 'clusters' to be present in projected frame" << std::endl;
    return 1;
  }
  // Unselected collection payload must NOT be available
  if (projFrame.get("hits") != nullptr) {
    std::cerr << "Expected 'hits' to be absent in projected frame payload" << std::endl;
    return 1;
  }

  // Complete collection ID mapping must still be preserved for unselected collections
  for (const auto& collName : fullFrame.getAvailableCollections()) {
    if (const auto* coll = fullFrame.get(collName)) {
      const auto collID = coll->getID();
      const auto resolvedName = projFrame.getName(collID);
      if (!resolvedName.has_value() || resolvedName.value() != collName) {
        std::cerr << "Failed to resolve collection name for ID " << collID << " ('" << collName
                  << "') on projected read with fresh reader" << std::endl;
        return 1;
      }
    }
  }

  return 0;
}

int test_arrow_reader_cache_expansion(const std::string& inputFile) {
  auto reader = podio::ArrowReader();
  reader.openFile(inputFile);

  // Initial read selecting only {"clusters"}
  auto frame1 = podio::Frame(reader.readEntry("events", 0, {"clusters"}));
  if (frame1.get("clusters") == nullptr || frame1.get("hits") != nullptr) {
    std::cerr << "Failed initial partial read" << std::endl;
    return 1;
  }

  // Expand selection to {"clusters", "hits"} - should reuse cached "clusters"
  auto frame2 = podio::Frame(reader.readEntry("events", 0, {"clusters", "hits"}));
  if (frame2.get("clusters") == nullptr || frame2.get("hits") == nullptr || frame2.get("mcparticles") != nullptr) {
    std::cerr << "Failed expanded partial read" << std::endl;
    return 1;
  }

  // Transition to full selection {} - should reuse existing cached columns
  auto frame3 = podio::Frame(reader.readEntry("events", 0, {}));
  if (frame3.get("clusters") == nullptr || frame3.get("hits") == nullptr || frame3.get("mcparticles") == nullptr) {
    std::cerr << "Failed full read after cache expansion" << std::endl;
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
      test_arrow_reader_invalid_coll(inputFile) + test_arrow_reader_edge_cases(inputFile) +
      test_arrow_reader_collection_id_mapping(inputFile) + test_arrow_reader_cache_expansion(inputFile);
}
