#include "podio/Frame.h"

#include "catch2/catch_test_macros.hpp"

#include "datamodel/ExampleClusterCollection.h"

TEST_CASE("Frame collections", "[frame][basics]") {
  auto event = podio::Frame();
  auto clusters = ExampleClusterCollection();
  clusters.create(3.14f);
  clusters.create(42.0f);

  event.put(std::move(clusters), "clusters");

  auto& coll = event.get<ExampleClusterCollection>("clusters");
  REQUIRE(coll[0].energy() == 3.14f);
  REQUIRE(coll[1].energy() == 42.0f);
}

TEST_CASE("Frame parameters", "[frame][basics]") {
  auto event = podio::Frame();

  event.putParameter("aString", "from a string literal");
  REQUIRE(event.getParameter<std::string>("aString") == "from a string literal");

  event.putParameter("someInts", {42, 123});
  const auto& ints = event.getParameter<std::vector<int>>("someInts");
  REQUIRE(ints.size() == 2);
  REQUIRE(ints[0] == 42);
  REQUIRE(ints[1] == 123);

  event.putParameter("someStrings", {"one", "two", "three"});
  const auto& strings = event.getParameter<std::vector<std::string>>("someStrings");
  REQUIRE(strings.size() == 3);
  REQUIRE(strings[0] == "one");
  REQUIRE(strings[1] == "two");
  REQUIRE(strings[2] == "three");
}
