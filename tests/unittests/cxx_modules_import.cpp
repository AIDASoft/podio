// SPDX-License-Identifier: Apache-2.0
// Test that validates direct C++20 module imports work correctly
//
// REQUIREMENTS:
// - CMake 3.28+ for CXX_MODULE file set support (module production)
// - CMake 3.30+ for improved module consumption from regular .cpp files  
// - Ninja or VS generator (Unix Makefiles doesn't support modules)
// - GCC 14+ or Clang 18+ with -fmodules-ts support
//
// CURRENT STATUS (Dec 2024):
// This test is DISABLED due to known issues with GCC 15 and Clang 18:
// - Standard library modules expose TU-local entities causing compilation errors
// - Module dependency scanning in CMake 3.31 doesn't fully handle std imports
// - These are upstream compiler/stdlib issues, not podio-specific
//
// FUTURE: This test demonstrates the INTENDED usage pattern when C++20 module
// support matures. It will be enabled once compilers provide stable standard
// library modules without TU-local entity issues. Until then, use traditional
// headers alongside generated modules for maximum compatibility.

import podio.core;
import datamodel.datamodel;

#include "catch2/catch_test_macros.hpp"

// Note: With proper module import, types should be accessible via their namespace
// However, we can bring them into scope for convenience
using namespace datamodel;

TEST_CASE("Direct import of podio.core module works", "[modules][import][core]") {
  // Test that basic podio.core types are accessible via import
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

TEST_CASE("Direct import of datamodel module works", "[modules][import][datamodel]") {
  // Test that datamodel types are accessible via import
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

TEST_CASE("Collections work via direct import", "[modules][import][collections]") {
  auto hits = ExampleHitCollection();
  
  auto hit1 = hits.create();
  hit1.energy(10.0);
  
  auto hit2 = hits.create();
  hit2.energy(20.0);
  
  REQUIRE(hits.size() == 2);
  REQUIRE(hits[0].energy() == 10.0);
  REQUIRE(hits[1].energy() == 20.0);
}

TEST_CASE("Relations work with direct imports", "[modules][import][relations]") {
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

TEST_CASE("Vector members work via direct import", "[modules][import][vectors]") {
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

TEST_CASE("Polymorphism works with imported modules", "[modules][import][polymorphism]") {
  auto hits = ExampleHitCollection();
  auto hit = hits.create();
  hit.energy(25.0);
  
  // Test that collection can be used polymorphically via podio.core interface
  podio::CollectionBase* basePtr = &hits;
  REQUIRE(basePtr->size() == 1);
  REQUIRE(basePtr->getTypeName() == "ExampleHitCollection");
  REQUIRE(basePtr->getValueTypeName() == "ExampleHit");
}

TEST_CASE("Mutable to immutable conversion works with imports", "[modules][import][mutability]") {
  auto hits = ExampleHitCollection();
  
  auto mutableHit = hits.create();
  mutableHit.energy(50.0);
  
  ExampleHit immutableHit = mutableHit;
  REQUIRE(immutableHit.energy() == 50.0);
  REQUIRE(immutableHit.isAvailable());
}

TEST_CASE("Integration between podio.core and datamodel imports", "[modules][import][integration]") {
  // This test verifies that types from both modules work together correctly
  auto coll = ExampleHitCollection();
  auto hit = coll.create();
  hit.energy(99.0);
  
  // Use podio.core interface with datamodel type
  podio::CollectionBase* base = &coll;
  REQUIRE(base->size() == 1);
  
  // Verify ObjectID works
  podio::ObjectID id{0, 0};
  REQUIRE(id.index == 0);
}

