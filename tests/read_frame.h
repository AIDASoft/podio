#ifndef PODIO_TESTS_READ_FRAME_H // NOLINT(llvm-header-guard): folder structure not suitable
#define PODIO_TESTS_READ_FRAME_H // NOLINT(llvm-header-guard): folder structure not suitable

#include "read_test.h"

#include "extension_model/ContainedTypeCollection.h"
#include "extension_model/ExternalComponentTypeCollection.h"
#include "extension_model/ExternalRelationTypeCollection.h"

#include "podio/Frame.h"

#include <iostream>

#define ASSERT(condition, msg)                                                                                         \
  if (!(condition)) {                                                                                                  \
    throw std::runtime_error(msg);                                                                                     \
  }

void processExtensions(const podio::Frame& event, int iEvent, podio::version::Version) {
  const auto& extColl = event.get<extension::ContainedTypeCollection>("extension_Contained");
  ASSERT(extColl.isValid(), "extension_Contained collection should be present");
  ASSERT(extColl.size() == 1, "extension_Contained collection should have one element");
  auto extElem = extColl[0];
  const auto& polVec = extElem.getAVector();
  ASSERT(polVec.r == iEvent * 123.f, "polVec.r value not as expected");
  ASSERT(polVec.phi == 0.15f, "polVec.phi value not as epxected");
  ASSERT(polVec.rho == 3.14f, "polVec.phi value not as epxected");

  const auto& extCompColl = event.get<extension::ExternalComponentTypeCollection>("extension_ExternalComponent");
  ASSERT(extCompColl.isValid(), "extension_ExternalComponent collection should be present");
  ASSERT(extCompColl.size() == 1, "extension_ExternalComponent should have one element");
  auto extCompElem = extCompColl[0];
  ASSERT((extCompElem.getAStruct().p == std::array{iEvent, iEvent - 2, iEvent + 4, iEvent * 8}),
         "aStruct.p value not as expected");
  ASSERT(extCompElem.getAComponent().aStruct.data.x == 42 * iEvent, "aComponent.aStruct.x value not as expected");
  ASSERT(extCompElem.getAComponent().nspStruct.x == iEvent, "aComponent.nspStruct.x value not as expected");
  ASSERT(extCompElem.getAComponent().nspStruct.y == iEvent * 2, "aComponent.nspStruct.y value not as expected");

  const auto& extRelColl = event.get<extension::ExternalRelationTypeCollection>("extension_ExternalRelation");
  ASSERT(extRelColl.isValid(), "extension_ExternalRelation collection should be present");
  ASSERT(extRelColl.size() == 3, "extension_ExternalRelation collection should contain 3 elements");

  const auto& hits = event.get<ExampleHitCollection>("hits");
  auto elem0 = extRelColl[0];
  ASSERT(elem0.getWeight() == iEvent * 100.f, "weight of first element not as expected");
  ASSERT(elem0.getSingleHit() == hits[0], "single hit relation not as expected");

  const auto& clusters = event.get<ExampleClusterCollection>("clusters");
  auto elem1 = extRelColl[1];
  const auto relClusters = elem1.getClusters();
  ASSERT(relClusters.size() == 2, "element should have two related clusters");
  ASSERT(relClusters[0] == clusters[1], "first related cluster not as expected");
  ASSERT(relClusters[1] == clusters[0], "first related cluster not as expected");

  auto elem2 = extRelColl[2];
  const auto& structs = elem2.getSomeStructs();
  ASSERT(structs.size() == 3, "element should have 3 struct vector members");
  ASSERT(structs[0].y == 0, "struct value not as expected");
  ASSERT(structs[1].y == iEvent, "struct value not as expected");
  ASSERT(structs[2].y == 2 * iEvent, "struct value not as expected");
}

template <typename ReaderT>
int read_frames(const std::string& filename, bool assertBuildVersion = true) {
  auto reader = ReaderT();
  try {
    reader.openFile(filename);
  } catch (const std::runtime_error& e) {
    std::cout << "File could not be opened, aborting." << std::endl;
    return 1;
  }

  if (assertBuildVersion && reader.currentFileVersion() != podio::version::build_version) {
    std::cerr << "The podio build version could not be read back correctly. "
              << "(expected:" << podio::version::build_version << ", actual: " << reader.currentFileVersion() << ")"
              << std::endl;
    return 1;
  }

  if (reader.getEntries("events") != 10) {
    std::cerr << "Could not read back the number of events correctly. "
              << "(expected:" << 10 << ", actual: " << reader.getEntries("events") << ")" << std::endl;
    return 1;
  }

  if (reader.getEntries("events") != reader.getEntries("other_events")) {
    std::cerr << "Could not read back the number of events correctly. "
              << "(expected:" << 10 << ", actual: " << reader.getEntries("other_events") << ")" << std::endl;
    return 1;
  }

  // Read the frames in a different order than when writing them here to make
  // sure that the writing/reading order does not impose any usage requirements
  for (size_t i = 0; i < reader.getEntries("events"); ++i) {
    auto frame = podio::Frame(reader.readNextEntry("events"));
    if (frame.get("emptySubsetColl") == nullptr) {
      std::cerr << "Could not retrieve an empty subset collection" << std::endl;
      return 1;
    }
    if (frame.get("emptyCollection") == nullptr) {
      std::cerr << "Could not retrieve an empty collection" << std::endl;
      return 1;
    }

    processEvent(frame, i, reader.currentFileVersion());

    auto otherFrame = podio::Frame(reader.readNextEntry("other_events"));
    processEvent(otherFrame, i + 100, reader.currentFileVersion());
    // The other_events category also holds external collections
    processExtensions(otherFrame, i + 100, reader.currentFileVersion());
  }

  if (reader.readNextEntry("events")) {
    std::cerr << "Trying to read more frame data than is present should return a nullptr" << std::endl;
    return 1;
  }

  std::cout << "========================================================\n" << std::endl;
  if (reader.readNextEntry("not_present")) {
    std::cerr << "Trying to read non-existant frame data should return a nullptr" << std::endl;
    return 1;
  }

  // Reading specific (jumping to) entry
  {
    auto frame = podio::Frame(reader.readEntry("events", 4));
    processEvent(frame, 4, reader.currentFileVersion());
    // Reading the next entry after jump, continues from after the jump
    auto nextFrame = podio::Frame(reader.readNextEntry("events"));
    processEvent(nextFrame, 5, reader.currentFileVersion());

    auto otherFrame = podio::Frame(reader.readEntry("other_events", 4));
    processEvent(otherFrame, 4 + 100, reader.currentFileVersion());
    processExtensions(otherFrame, 4 + 100, reader.currentFileVersion());

    // Jumping back also works
    auto previousFrame = podio::Frame(reader.readEntry("other_events", 2));
    processEvent(previousFrame, 2 + 100, reader.currentFileVersion());
    processExtensions(previousFrame, 2 + 100, reader.currentFileVersion());

    // Trying to read a Frame that is not present returns a nullptr
    if (reader.readEntry("events", 10)) {
      std::cerr << "Trying to read a specific entry that does not exist should return a nullptr" << std::endl;
      return 1;
    }
  }

  return 0;
}

#endif // PODIO_TESTS_READ_FRAME_H
