#include "datamodel/ExampleClusterCollection.h"
#include "datamodel/ExampleHitCollection.h"
#include "datamodel/TestLinkCollection.h"

#include <iostream>
#include <stdexcept>

void test_root_macro() {
  auto success = true;
  const auto check = [&success](const char* name, const auto& actual, const auto& expected) {
    if (actual == expected) {
      return;
    }

    std::cerr << "test_root_macro: " << name << " failed: expected " << expected << ", got " << actual << '\n';
    success = false;
  };

  // Link collection
  auto linkCol = TestLinkCollection{};
  auto link = linkCol.create();
  link.setWeight(0.5);
  check("link weight", link.getWeight(), 0.5);
  check("link collection size", linkCol.size(), 1u);

  // Regular datatype collections
  auto hitCol = ExampleHitCollection{};
  auto hit = hitCol.create();
  hit.energy(42.0);
  check("hit energy", hit.energy(), 42.0);
  check("hit collection size", hitCol.size(), 1u);

  auto clusterCol = ExampleClusterCollection{};
  auto cluster = clusterCol.create();
  cluster.energy(7.0);
  check("cluster energy", cluster.energy(), 7.0);
  check("cluster collection size", clusterCol.size(), 1u);

  if (!success) {
    throw std::runtime_error("test_root_macro failed");
  }
}
