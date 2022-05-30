#include "write_frame.h"

#include "podio/ROOTFrameWriter.h"

#include <string>
#include <vector>

static const std::vector<std::string> collsToWrite = {"mcparticles",
                                                      "moreMCs",
                                                      "arrays",
                                                      "mcParticleRefs",
                                                      "strings",
                                                      "hits",
                                                      "hitRefs",
                                                      "refs",
                                                      "refs2",
                                                      "clusters",
                                                      "OneRelation",
                                                      "info",
                                                      "WithVectorMember",
                                                      "fixedWidthInts",
                                                      "userInts",
                                                      "userDoubles",
                                                      "WithNamespaceMember",
                                                      "WithNamespaceRelation",
                                                      "WithNamespaceRelationCopy"};

int main(int, char**) {
  auto writer = podio::ROOTFrameWriter("example_frame.root");

  for (int i = 0; i < 10; ++i) {
    auto frame = makeFrame(i);
    writer.writeFrame(frame, "events", collsToWrite);
  }

  for (int i = 100; i < 111; ++i) {
    auto frame = makeFrame(i);
    writer.writeFrame(frame, "other_events");
  }

  writer.finish();
  return 0;
}
