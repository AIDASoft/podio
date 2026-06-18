#include "datamodel/TestLinkCollection.h"
#include "datamodel/ExampleHitCollection.h"
#include "datamodel/ExampleClusterCollection.h"

void test_root_macro() {
    // Link collection
    auto linkCol = TestLinkCollection{};
    auto link = linkCol.create();
    link.setWeight(0.5);
    if (link.getWeight() != 0.5) exit(1);
    if (linkCol.size() != 1) exit(1);

    // Regular datatype collections
    auto hitCol = ExampleHitCollection{};
    auto hit = hitCol.create();
    hit.energy(42.0);
    if (hit.energy() != 42.0) exit(1);
    if (hitCol.size() != 1) exit(1);

    auto clusterCol = ExampleClusterCollection{};
    auto cluster = clusterCol.create();
    cluster.energy(7.0);
    if (cluster.energy() != 7.0) exit(1);
    if (clusterCol.size() != 1) exit(1);
}
