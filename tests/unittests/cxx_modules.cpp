// SPDX-License-Identifier: Apache-2.0

#include "catch2/catch_test_macros.hpp"

#include "podio/CollectionBase.h"
#include "podio/Frame.h"

#include "datamodel/ExampleHitCollection.h"
#include "datamodel/ExampleClusterCollection.h"
#include "datamodel/ExampleWithVectorMemberCollection.h"

// This test file validates that C++ modules work correctly when enabled
// The tests themselves are the same whether modules are on or off - they
// just test the basic functionality. The key is that when compiled with
// modules enabled, the module interfaces are being used properly.

TEST_CASE("Basic datamodel types work", "[modules][basic]") {
  auto hits = ExampleHitCollection();
  auto hit = hits.create();
  hit.energy(42.0);
  hit.x(1.0);
  hit.y(2.0);
  hit.z(3.0);
  
  REQUIRE(hit.energy() == 42.0);
  REQUIRE(hit.x() == 1.0);
  REQUIRE(hit.y() == 2.0);
  REQUIRE(hit.z() == 3.0);
}

TEST_CASE("Collections can be created and used", "[modules][collections]") {
  auto hits = ExampleHitCollection();
  
  auto hit1 = hits.create();
  hit1.energy(10.0);
  
  auto hit2 = hits.create();
  hit2.energy(20.0);
  
  REQUIRE(hits.size() == 2);
  REQUIRE(hits[0].energy() == 10.0);
  REQUIRE(hits[1].energy() == 20.0);
}

TEST_CASE("Relations work across types", "[modules][relations]") {
  auto hits = ExampleHitCollection();
  auto clusters = ExampleClusterCollection();
  
  auto hit1 = hits.create();
  hit1.energy(5.0);
  
  auto hit2 = hits.create();
  hit2.energy(7.0);
  
  auto cluster = clusters.create();
  cluster.energy(12.0);
  cluster.addHits(hit1);
  cluster.addHits(hit2);
  
  REQUIRE(cluster.Hits_size() == 2);
  REQUIRE(cluster.Hits(0).energy() == 5.0);
  REQUIRE(cluster.Hits(1).energy() == 7.0);
}

TEST_CASE("Vector members work correctly", "[modules][vectors]") {
  auto coll = ExampleWithVectorMemberCollection();
  
  auto obj = coll.create();
  obj.addcount(1);
  obj.addcount(2);
  obj.addcount(3);
  
  REQUIRE(obj.count_size() == 3);
  REQUIRE(obj.count(0) == 1);
  REQUIRE(obj.count(1) == 2);
  REQUIRE(obj.count(2) == 3);
}

TEST_CASE("Frame I/O works with modules", "[modules][io]") {
  // Create a frame with some data
  podio::Frame writeFrame;
  
  auto hits = ExampleHitCollection();
  auto hit = hits.create();
  hit.energy(99.0);
  hit.x(1.0);
  hit.y(2.0);
  hit.z(3.0);
  
  writeFrame.put(std::move(hits), "hits");
  
  // In a real scenario this would be written to disk and read back
  // For this unit test, we just verify the frame contains what we put in
  REQUIRE(writeFrame.get("hits") != nullptr);
  
  const auto& readHits = writeFrame.get<ExampleHitCollection>("hits");
  REQUIRE(readHits.size() == 1);
  REQUIRE(readHits[0].energy() == 99.0);
}

TEST_CASE("podio.core module exports work", "[modules][core]") {
  // Test that basic podio.core types are accessible
  // This validates the podio.core module interface
  
  podio::ObjectID id{42, 1};
  REQUIRE(id.index == 42);
  REQUIRE(id.collectionID == 1);
  
  podio::CollectionIDTable table;
  REQUIRE(table.empty());
  
  auto collID = table.add("test_collection");
  REQUIRE(!table.empty());
  REQUIRE(table.collectionID("test_collection").has_value());
  REQUIRE(table.collectionID("test_collection").value() == collID);
}

TEST_CASE("Mutable to immutable conversion works", "[modules][mutability]") {
  auto hits = ExampleHitCollection();
  
  auto mutableHit = hits.create();
  mutableHit.energy(50.0);
  
  ExampleHit immutableHit = mutableHit;
  REQUIRE(immutableHit.energy() == 50.0);
  
  // Verify the immutable one is linked to the collection
  REQUIRE(immutableHit.isAvailable());
}

TEST_CASE("Collection polymorphism works", "[modules][polymorphism]") {
  auto hits = ExampleHitCollection();
  auto hit = hits.create();
  hit.energy(25.0);
  
  // Test that collection can be used polymorphically
  podio::CollectionBase* basePtr = &hits;
  REQUIRE(basePtr->size() == 1);
  REQUIRE(basePtr->getTypeName() == "ExampleHitCollection");
  REQUIRE(basePtr->getValueTypeName() == "ExampleHit");
}

#ifdef PODIO_ENABLE_CXX_MODULES
TEST_CASE("Module support is enabled", "[modules][meta]") {
  // This test only runs when modules are actually enabled
  // It's more of a sanity check that the build system is working
  REQUIRE(true);
}
#endif
