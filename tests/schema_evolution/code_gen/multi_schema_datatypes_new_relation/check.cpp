#include "datamodel/ExampleClusterCollection.h"
#include "datamodel/ExampleHitCollection.h"

#include "check_base.h"

#ifdef PODIO_SCHEMA_EVOLUTION_TEST_READ
// Checks everything that is present in all versions of the datamodel
int checkCommon(const ExampleClusterCollection& clusters) {
  ASSERT_EQUAL(clusters.size(), 3, "Collection with new relations should be readable");
  const auto c0 = clusters[0];
  const auto c1 = clusters[1];
  const auto c2 = clusters[2];
  ASSERT_EQUAL(c0.num(), 10, "Member variables unrelated to schema evolution have changed");
  ASSERT_EQUAL(c0.Hits().size(), 2, "Existing relation should be unchanged");
  ASSERT_EQUAL(c0.Hits()[0].num(), 1, "Existing relation should be unchanged");
  ASSERT_EQUAL(c0.Hits()[1].num(), 2, "Existing relation should be unchanged");
  ASSERT_EQUAL(c1.Hits().size(), 1, "Existing relation should be unchanged");
  ASSERT_EQUAL(c1.Hits()[0].num(), 3, "Existing relation should be unchanged");
  ASSERT_EQUAL(c2.Hits().size(), 0, "Existing relation should be unchanged");
  ASSERT_EQUAL(c0.Parent().isAvailable(), false, "Existing OneToOne relation should be unchanged");
  ASSERT_EQUAL(c1.Parent().isAvailable(), true, "Existing OneToOne relation should be unchanged");
  ASSERT_EQUAL(c1.Parent().num(), 10, "Existing OneToOne relation should be unchanged");
  for (const auto c : clusters) {
    ASSERT_EQUAL(c.OtherHits().size(), 0, "New relation should be empty");
    ASSERT_EQUAL(c.Siblings().size(), 0, "New relation should be empty");
  }
  return 0;
}
#endif

int main() {
  WRITE_AS_OLDEST(ExampleClusterCollection, {
    auto hits = ExampleHitCollection();
    auto h0 = hits.create();
    h0.num(1);
    auto h1 = hits.create();
    h1.num(2);
    auto h2 = hits.create();
    h2.num(3);

    elem.num(10);
    elem.addHits(h0);
    elem.addHits(h1);
    auto c1 = coll.create();
    c1.num(20);
    c1.addHits(h2);
    c1.Parent(elem);
    auto c2 = coll.create();
    c2.num(30);

    event.put(std::move(hits), "hits");
  });

  WRITE_AS_OLD(ExampleClusterCollection, {
    auto hits = ExampleHitCollection();
    auto h0 = hits.create();
    h0.num(1);
    auto h1 = hits.create();
    h1.num(2);
    auto h2 = hits.create();
    h2.num(3);

    elem.num(10);
    elem.addHits(h0);
    elem.addHits(h1);
    auto c1 = coll.create();
    c1.num(20);
    c1.addHits(h2);
    c1.Parent(elem);
    auto c2 = coll.create();
    c2.num(30);
    elem.addClusters(c1);
    elem.addClusters(c2);
    c1.addClusters(elem);

    event.put(std::move(hits), "hits");
  });

  READ_AS_FROM_VERSION(ExampleClusterCollection, "oldest", {
    if (checkCommon(coll)) {
      return 1;
    }
    for (const auto c : coll) {
      ASSERT_EQUAL(c.Clusters().size(), 0, "New relation should be empty");
    }
  });

  READ_AS_FROM_VERSION(ExampleClusterCollection, "old", {
    if (checkCommon(coll)) {
      return 1;
    }
    ASSERT_EQUAL(coll[0].Clusters().size(), 2, "Relation of the old version should be unchanged");
    ASSERT_EQUAL(coll[0].Clusters()[0].num(), 20, "Relation of the old version should be unchanged");
    ASSERT_EQUAL(coll[0].Clusters()[1].num(), 30, "Relation of the old version should be unchanged");
    ASSERT_EQUAL(coll[1].Clusters().size(), 1, "Relation of the old version should be unchanged");
    ASSERT_EQUAL(coll[1].Clusters()[0].num(), 10, "Relation of the old version should be unchanged");
    ASSERT_EQUAL(coll[2].Clusters().size(), 0, "Relation of the old version should be unchanged");
  });

  return 0;
}
