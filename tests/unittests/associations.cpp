#include "catch2/catch_test_macros.hpp"

#include "podio/Association.h"

#include "datamodel/ExampleCluster.h"
#include "datamodel/ExampleHit.h"

// Test datatypes

TEST_CASE("Association basics") {
  using TestA = podio::Association<ExampleHit, ExampleCluster>;

  auto assoc = TestA();

  REQUIRE(true);
}
