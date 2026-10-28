#include "datamodel/ExampleClusterCollection.h"

#include "check_base.h"

int main() {
  WRITE_WITH(WriterT, (TEST_CASE FILE_SUFFIX), {
    auto clusters = ExampleClusterCollection();
    auto parent = clusters.create();
    parent.num(10);
    auto child = clusters.create();
    child.num(20);
    child.Parent(parent);
    auto orphan = clusters.create();
    orphan.num(30);

    event.put(std::move(clusters), "evolution_collection");
  })

  READ_WITH(ReaderT, (TEST_CASE FILE_SUFFIX), {
    const auto& clusters = event.get<ExampleClusterCollection>("evolution_collection");
    ASSERT_EQUAL(clusters.size(), 3, "Collection with a new relation should be readable");
    ASSERT_EQUAL(clusters[1].num(), 20, "Member variables unrelated to schema evolution have changed");
    ASSERT_EQUAL(clusters[0].Parent().isAvailable(), false, "Existing OneToOne relation should be unchanged");
    ASSERT_EQUAL(clusters[1].Parent().isAvailable(), true, "Existing OneToOne relation should be unchanged");
    ASSERT_EQUAL(clusters[1].Parent().num(), 10, "Existing OneToOne relation should be unchanged");
    ASSERT_EQUAL(clusters[2].Parent().isAvailable(), false, "Existing OneToOne relation should be unchanged");
    for (const auto cluster : clusters) {
      ASSERT_EQUAL(cluster.Hits().size(), 0, "New relation should be empty");
    }
  })
}
