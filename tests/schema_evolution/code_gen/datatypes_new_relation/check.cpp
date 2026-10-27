#include "datamodel/ExampleClusterCollection.h"
#include "datamodel/ExampleHitCollection.h"

#include "check_base.h"

int main() {
  WRITE_WITH(WriterT, (TEST_CASE FILE_SUFFIX), {
    auto hits = ExampleHitCollection();
    auto hit = hits.create();
    hit.x(1.23);

    auto clusters = ExampleClusterCollection();
    auto cluster = clusters.create();
    cluster.energy(4.56);
    cluster.addHits(hit);
    auto subCluster = clusters.create();
    subCluster.energy(7.89);
    cluster.addClusters(subCluster);

    event.put(std::move(hits), "related_collection");
    event.put(std::move(clusters), "evolution_collection");
  })

  READ_WITH(ReaderT, (TEST_CASE FILE_SUFFIX), {
    const auto& clusterColl = event.get<ExampleClusterCollection>("evolution_collection");
    ASSERT_EQUAL(clusterColl.size(), 2, "Collection with a new relation should be readable");
    const auto cluster = clusterColl[0];
    ASSERT_EQUAL(cluster.energy(), 4.56, "Member variables unrelated to schema evolution have changed");
    ASSERT_EQUAL(cluster.Hits().size(), 1, "Existing relation should be unchanged");
    ASSERT_EQUAL(cluster.Hits()[0].x(), 1.23, "Existing relation should be unchanged");
    ASSERT_EQUAL(cluster.OtherHits().size(), 0, "New relation should be empty");
    ASSERT_EQUAL(cluster.Clusters().size(), 1, "Existing relation after the new one should be unchanged");
    ASSERT_EQUAL(cluster.Clusters()[0].energy(), 7.89, "Existing relation after the new one should be unchanged");
    ASSERT_EQUAL(clusterColl[1].OtherHits().size(), 0, "New relation should be empty");
  })
}
