// SPDX-License-Identifier: Apache-2.0
// Simple module import test without Catch2
//
// This test validates that C++20 module imports work correctly by:
// - Importing podio.core module
// - Importing datamodel module
// - Using types from both modules
// - Performing basic operations
//
// Returns 0 on success, non-zero on failure

import podio.core;
import datamodel.datamodel;

#include <iostream>
#include <cstdlib>

int main() {
  int failures = 0;

  std::cout << "Testing podio.core module imports..." << std::endl;

  // Test 1: podio::ObjectID
  podio::ObjectID id{42, 1};
  if (id.index != 42 || id.collectionID != 1) {
    std::cerr << "FAILED: ObjectID construction" << std::endl;
    failures++;
  } else {
    std::cout << "PASSED: ObjectID construction" << std::endl;
  }

  // Test 2: podio::CollectionIDTable
  podio::CollectionIDTable table;
  if (!table.empty()) {
    std::cerr << "FAILED: CollectionIDTable should be empty" << std::endl;
    failures++;
  } else {
    std::cout << "PASSED: CollectionIDTable empty check" << std::endl;
  }

  auto collID = table.add("test_collection");
  if (table.empty()) {
    std::cerr << "FAILED: CollectionIDTable should not be empty after add" << std::endl;
    failures++;
  } else {
    std::cout << "PASSED: CollectionIDTable add" << std::endl;
  }

  auto retrieved = table.collectionID("test_collection");
  if (!retrieved.has_value() || retrieved.value() != collID) {
    std::cerr << "FAILED: CollectionIDTable retrieval" << std::endl;
    failures++;
  } else {
    std::cout << "PASSED: CollectionIDTable retrieval" << std::endl;
  }

  std::cout << "\nTesting datamodel module imports..." << std::endl;

  // Test 3: ExampleHit from datamodel
  auto hits = datamodel::ExampleHitCollection();
  auto hit = hits.create();
  hit.energy(42.5);
  hit.x(1.0);
  hit.y(2.0);
  hit.z(3.0);

  if (hit.energy() != 42.5 || hit.x() != 1.0 || hit.y() != 2.0 || hit.z() != 3.0) {
    std::cerr << "FAILED: ExampleHit setters/getters" << std::endl;
    failures++;
  } else {
    std::cout << "PASSED: ExampleHit setters/getters" << std::endl;
  }

  // Test 4: Collection operations
  auto hit2 = hits.create();
  hit2.energy(100.0);

  if (hits.size() != 2) {
    std::cerr << "FAILED: Collection size should be 2, got " << hits.size() << std::endl;
    failures++;
  } else {
    std::cout << "PASSED: Collection size" << std::endl;
  }

  if (hits[0].energy() != 42.5 || hits[1].energy() != 100.0) {
    std::cerr << "FAILED: Collection indexing" << std::endl;
    failures++;
  } else {
    std::cout << "PASSED: Collection indexing" << std::endl;
  }

  // Test 5: Relations
  auto clusters = datamodel::ExampleClusterCollection();
  auto cluster = clusters.create();
  cluster.energy(142.5);
  cluster.addHits(hit);
  cluster.addHits(hit2);

  if (cluster.Hits_size() != 2) {
    std::cerr << "FAILED: Cluster should have 2 hits" << std::endl;
    failures++;
  } else {
    std::cout << "PASSED: Cluster relations" << std::endl;
  }

  if (cluster.Hits(0).energy() != 42.5 || cluster.Hits(1).energy() != 100.0) {
    std::cerr << "FAILED: Cluster hit energies" << std::endl;
    failures++;
  } else {
    std::cout << "PASSED: Cluster hit access" << std::endl;
  }

  // Test 6: Polymorphism via podio.core interfaces
  podio::CollectionBase* basePtr = &hits;
  if (basePtr->size() != 2) {
    std::cerr << "FAILED: Polymorphic size check" << std::endl;
    failures++;
  } else {
    std::cout << "PASSED: Polymorphic interface" << std::endl;
  }

  if (basePtr->getTypeName() != "ExampleHitCollection") {
    std::cerr << "FAILED: Type name should be ExampleHitCollection" << std::endl;
    failures++;
  } else {
    std::cout << "PASSED: Type name retrieval" << std::endl;
  }

  // Summary
  std::cout << "\n===============================================" << std::endl;
  if (failures == 0) {
    std::cout << "All module import tests PASSED" << std::endl;
    std::cout << "===============================================" << std::endl;
    return EXIT_SUCCESS;
  } else {
    std::cout << "Module import tests FAILED: " << failures << " failures" << std::endl;
    std::cout << "===============================================" << std::endl;
    return EXIT_FAILURE;
  }
}
