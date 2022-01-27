#include "catch2/catch_test_macros.hpp"

#include "podio/Association.h"

#include "datamodel/ExampleCluster.h"
#include "datamodel/ExampleHit.h"
#include "datamodel/MutableExampleCluster.h"
#include "datamodel/MutableExampleHit.h"

#include <type_traits>

// Test datatypes
using TestA = podio::Association<ExampleHit, ExampleCluster>;
using TestMA = podio::MutableAssociation<ExampleHit, ExampleCluster>;

TEST_CASE("Association constness", "[associations][static-checks]") {
  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<TestMA>().getFrom()), ExampleHit>);
  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<TestMA>().getTo()), ExampleCluster>);

  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<TestA>().getFrom()), ExampleHit>);
  STATIC_REQUIRE(std::is_same_v<decltype(std::declval<TestA>().getTo()), ExampleCluster>);
}

TEST_CASE("Association basics", "[associations]") {
  auto cluster = MutableExampleCluster();
  auto hit = MutableExampleHit();

  auto mutAssoc = TestMA();
  mutAssoc.setWeight(3.14f);
  mutAssoc.setFrom(hit);
  mutAssoc.setTo(cluster);

  REQUIRE(mutAssoc.getWeight() == 3.14f);
  REQUIRE(mutAssoc.getFrom() == hit);
  REQUIRE(mutAssoc.getTo() == cluster);

  SECTION("Copying") {
    auto otherAssoc = mutAssoc;
    REQUIRE(otherAssoc.getWeight() == 3.14f);
    REQUIRE(otherAssoc.getFrom() == hit);
    REQUIRE(otherAssoc.getTo() == cluster);

    auto otherCluster = ExampleCluster();
    auto otherHit = ExampleHit();
    otherAssoc.setFrom(otherHit);
    otherAssoc.setTo(otherCluster);
    otherAssoc.setWeight(42.0f);
    REQUIRE(otherAssoc.getWeight() == 42.0f);
    REQUIRE(otherAssoc.getFrom() == otherHit);
    REQUIRE(otherAssoc.getTo() == otherCluster);

    // Make sure original association changes as well
    REQUIRE(mutAssoc.getWeight() == 42.0f);
    REQUIRE(mutAssoc.getFrom() == otherHit);
    REQUIRE(mutAssoc.getTo() == otherCluster);
  }

  SECTION("Assignment") {
    auto otherAssoc = TestMA();
    otherAssoc = mutAssoc;
    REQUIRE(otherAssoc.getWeight() == 3.14f);
    REQUIRE(otherAssoc.getFrom() == hit);
    REQUIRE(otherAssoc.getTo() == cluster);

    auto otherCluster = ExampleCluster();
    auto otherHit = ExampleHit();
    otherAssoc.setFrom(otherHit);
    otherAssoc.setTo(otherCluster);
    otherAssoc.setWeight(42.0f);
    REQUIRE(otherAssoc.getWeight() == 42.0f);
    REQUIRE(otherAssoc.getFrom() == otherHit);
    REQUIRE(otherAssoc.getTo() == otherCluster);

    // Make sure original association changes as well
    REQUIRE(mutAssoc.getWeight() == 42.0f);
    REQUIRE(mutAssoc.getFrom() == otherHit);
    REQUIRE(mutAssoc.getTo() == otherCluster);
  }

  SECTION("Implicit conversion") {
    // Use an immediately invoked lambda to check that the implicit conversion
    // is working as desired
    [hit, cluster](TestA assoc) {
      REQUIRE(assoc.getWeight() == 3.14f);
      REQUIRE(assoc.getFrom() == hit);
      REQUIRE(assoc.getTo() == cluster);
    }(mutAssoc);
  }

  SECTION("Cloning") {
    auto otherAssoc = mutAssoc.clone();
    REQUIRE(otherAssoc.getWeight() == 3.14f);
    REQUIRE(otherAssoc.getFrom() == hit);
    REQUIRE(otherAssoc.getTo() == cluster);

    auto otherCluster = ExampleCluster();
    auto otherHit = ExampleHit();
    otherAssoc.setFrom(otherHit);
    otherAssoc.setTo(otherCluster);
    otherAssoc.setWeight(42.0f);
    REQUIRE(otherAssoc.getWeight() == 42.0f);
    REQUIRE(otherAssoc.getFrom() == otherHit);
    REQUIRE(otherAssoc.getTo() == otherCluster);

    // Make sure original association is unchanged
    REQUIRE(mutAssoc.getWeight() == 3.14f);
    REQUIRE(mutAssoc.getFrom() == hit);
    REQUIRE(mutAssoc.getTo() == cluster);

    // Check cloning from an immutable one
    TestA assoc = mutAssoc;
    auto anotherAssoc = assoc.clone();
    anotherAssoc.setFrom(otherHit);
    anotherAssoc.setTo(otherCluster);
    anotherAssoc.setWeight(42.0f);
    REQUIRE(anotherAssoc.getWeight() == 42.0f);
    REQUIRE(anotherAssoc.getFrom() == otherHit);
    REQUIRE(anotherAssoc.getTo() == otherCluster);
  }

  SECTION("Equality operator") {
    auto otherAssoc = mutAssoc;
    REQUIRE(otherAssoc == mutAssoc);

    // Mutable and immutable associations should be comparable
    TestA assoc = mutAssoc;
    REQUIRE(assoc == mutAssoc);
  }
}
