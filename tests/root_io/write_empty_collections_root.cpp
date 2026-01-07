#include "datamodel/ExampleHitCollection.h"

#include "podio/Frame.h"
#include "podio/ROOTReader.h"
#include "podio/ROOTWriter.h"

#include <iostream>
#include <string>
#include <vector>

namespace {

int checkEmptyCollectionsFrame(const podio::Frame& frame) {
  const auto colls = frame.getAvailableCollections();
  if (!colls.empty()) {
    std::cerr << "expected no collections, got " << colls.size() << std::endl;
    return 1;
  }

  if (frame.get("hits") != nullptr) {
    std::cerr << "collection 'hits' should not be persisted" << std::endl;
    return 1;
  }

  const auto anInt = frame.getParameter<int>("an_int");
  if (!anInt.has_value() || anInt.value() != 42) {
    std::cerr << "parameter an_int not stored correctly" << std::endl;
    return 1;
  }

  const auto greetings = frame.getParameter<std::vector<std::string>>("greetings");
  const std::vector<std::string> expectedGreetings{"from", "python"};
  if (!greetings.has_value() || greetings.value() != expectedGreetings) {
    std::cerr << "parameter greetings not stored correctly" << std::endl;
    return 1;
  }

  return 0;
}

} // namespace

int main(int, char**) {
  const auto filename = std::string{"empty_colls_frame_cpp.root"};

  podio::Frame frame;
  auto hits = ExampleHitCollection();
  hits.create(0xBADull, 0.0f, 0.0f, 0.0f, 23.0f);
  frame.put(std::move(hits), "hits");

  frame.putParameter("an_int", 42);
  frame.putParameter("greetings", std::vector<std::string>{"from", "python"});

  auto writer = podio::ROOTWriter(filename);
  const std::vector<std::string> noCollections{};
  writer.writeFrame(frame, "events", noCollections);
  writer.finish();

  auto reader = podio::ROOTReader();
  reader.openFile(filename);

  if (reader.getEntries("events") != 1) {
    std::cerr << "expected exactly one entry" << std::endl;
    return 1;
  }

  auto data = reader.readEntry("events", 0);
  if (!data) {
    std::cerr << "could not read entry 0" << std::endl;
    return 1;
  }

  return checkEmptyCollectionsFrame(podio::Frame(std::move(data)));
}
