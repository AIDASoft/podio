#include "podio/Frame.h"

#include "catch2/catch_test_macros.hpp"

#include "datamodel/ExampleClusterCollection.h"

TEST_CASE("Frame", "[frame][basics]") {
  auto event = podio::Frame();
  auto clusters = ExampleClusterCollection();
  clusters.create(3.14f);
  clusters.create(42.0f);

  event.put(std::move(clusters), "clusters");

  auto& coll = event.get<ExampleClusterCollection>("clusters");
  REQUIRE(coll[0].energy() == 3.14f);
  REQUIRE(coll[1].energy() == 42.0f);
}
