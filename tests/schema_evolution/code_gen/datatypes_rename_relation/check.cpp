#include "datamodel/ExampleClusterCollection.h"
#include "datamodel/ExampleHitCollection.h"

#include "check_base.h"

int main() {
  WRITE_WITH(WriterT, (TEST_CASE FILE_SUFFIX), {
    auto hits = ExampleHitCollection();
    auto hit = hits.create();
    hit.x(1.23);
    hit.y(2.34);
    hit.z(3.45);
    hit.energy(5.67);

    auto clusters = ExampleClusterCollection();
    auto cluster = clusters.create();
    cluster.addhits(hit);
    cluster.addoldHits(hit);
    cluster.addweights(0.5f);

    event.put(std::move(hits), "related_collection");
    event.put(std::move(clusters), "evolution_collection");
  })

  READ_WITH(ReaderT, (TEST_CASE FILE_SUFFIX), {
    // ASSERT_EQUAL returns 1 on failure, which is propagated to main
    const auto checkEvent = [](const podio::Frame& frame) -> int {
      const auto& hitColl = frame.get<ExampleHitCollection>("related_collection");
      ASSERT_EQUAL(hitColl.hasID(), true, "Readable collection should be available");
      ASSERT_EQUAL(hitColl[0].x(), 1.23, "Readable elements should not change");
      ASSERT_EQUAL(hitColl[0].y(), 2.34, "Readable elements should not change");
      ASSERT_EQUAL(hitColl[0].z(), 3.45, "Readable elements should not change");
      ASSERT_EQUAL(hitColl[0].energy(), 5.67, "Readable elements should not change");

      try {
        const auto& clusterColl [[maybe_unused]] = frame.get<ExampleClusterCollection>("evolution_collection");
        // We should not get here, we just use the ASSERT_EQUAL for the easy message emitting
        ASSERT_EQUAL(true, false, "Unreadable collection should be skipped");
      } catch (const std::runtime_error&) {
        ASSERT_EQUAL(true, true, "Unreadable collection should be skipped");
      }
      return 0;
    };
    if (checkEvent(event) != 0) {
      return 1;
    }

    // RNTuple: Reading again goes through the remembered binding failure instead of
    // trying to bind the collection again
    const auto repeated = podio::Frame(reader.readEntry(podio::Category::Event, 0));
    if (checkEvent(repeated) != 0) {
      return 1;
    }
  })
}
