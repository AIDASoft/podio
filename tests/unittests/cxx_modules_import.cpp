// SPDX-License-Identifier: Apache-2.0
// Test that validates C++ module infrastructure is working
// This file tests that when modules are enabled, the generated code still compiles
// and works correctly, even when consumed via traditional headers

#include "catch2/catch_test_macros.hpp"

#include "podio/CollectionBase.h"
#include "podio/CollectionIDTable.h"
#include "datamodel/ExampleHitCollection.h"
#include "datamodel/ExampleClusterCollection.h"

// These tests verify that module-enabled builds work correctly
// The key is that the datamodel was compiled with modules enabled,
// and we can still use it via traditional includes

TEST_CASE("Module-compiled datamodel works via includes", "[modules][headers]") {
  // Create objects using the datamodel that was compiled with modules
  auto hits = ExampleHitCollection();
  
  auto hit = hits.create();
  hit.energy(123.45);
  hit.x(1.1);
  hit.y(2.2);
  hit.z(3.3);
  
  REQUIRE(hits.size() == 1);
  REQUIRE(hits[0].energy() == 123.45);
  REQUIRE(hits[0].x() == 1.1);
  REQUIRE(hits[0].y() == 2.2);
  REQUIRE(hits[0].z() == 3.3);
}

TEST_CASE("podio.core types work in module build", "[modules][core]") {
  // Test basic types that are part of podio.core module
  podio::ObjectID id{100, 5};
  REQUIRE(id.index == 100);
  REQUIRE(id.collectionID == 5);
  
  podio::CollectionIDTable table;
  auto collID = table.add("my_collection");
  REQUIRE(table.collectionID("my_collection").has_value());
  REQUIRE(*table.collectionID("my_collection") == collID);
}

TEST_CASE("Collections work with podio.core types", "[modules][integration]") {
  // Test that datamodel types work with podio.core interfaces
  auto coll = ExampleHitCollection();
  auto hit = coll.create();
  hit.energy(50.0);
  
  // Use podio.core interface
  podio::CollectionBase* base = &coll;
  REQUIRE(base->size() == 1);
  REQUIRE(base->getTypeName() == "ExampleHitCollection");
}

TEST_CASE("Relations work in module build", "[modules][relations]") {
  auto hits = ExampleHitCollection();
  auto clusters = ExampleClusterCollection();
  
  auto h1 = hits.create();
  h1.energy(10.0);
  
  auto h2 = hits.create();
  h2.energy(15.0);
  
  auto cluster = clusters.create();
  cluster.energy(25.0);
  cluster.addHits(h1);
  cluster.addHits(h2);
  
  REQUIRE(cluster.Hits_size() == 2);
  
  float totalEnergy = 0.0;
  for (const auto& hit : cluster.Hits()) {
    totalEnergy += hit.energy();
  }
  REQUIRE(totalEnergy == 25.0);
}
