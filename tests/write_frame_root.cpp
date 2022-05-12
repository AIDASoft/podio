#include "write_frame.h"

#include "podio/ROOTFrameWriter.h"

#include <array>
#include <string_view>

static constexpr std::array<std::string_view, 20> collsToWrite = {
    "mcparticles",      "moreMCs",        "arrays",   "mcParticleRefs", "strings",     "hits",
    "hitRefs",          "refs",           "refs2",    "clusters",       "OneRelation", "info",
    "WithVectorMember", "fixedWidthInts", "userInts", "userDoubles"};

int main(int, char**) {
  auto writer = podio::ROOTFrameWriter("example_frame.root");

  for (const auto n : collsToWrite) {
    if (!n.empty()) {
      writer.registerForWrite(std::string(n));
    }
  }

  for (int i = 0; i < 10; ++i) {
    auto frame = makeFrame(i);
    writer.writeFrame(frame);
  }

  writer.finish();
  return 0;
}
