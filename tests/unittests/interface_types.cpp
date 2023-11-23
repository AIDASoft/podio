#include "catch2/catch_test_macros.hpp"

#include "podio/ObjectID.h"

#include "datamodel/AnotherTypeWithEnergy.h"
#include "datamodel/EnergyInNamespaceCollection.h"
#include "datamodel/ExampleClusterCollection.h"
#include "datamodel/ExampleHitCollection.h"
#include "datamodel/TypeWithEnergy.h"
#include <stdexcept>

TEST_CASE("InterfaceTypes basic functionality", "[interface-types][basics]") {
  using WrapperT = TypeWithEnergy;

  auto emptyWrapper = WrapperT::makeEmpty();
  REQUIRE_FALSE(emptyWrapper.isAvailable());
  REQUIRE(emptyWrapper.holds<ExampleHit>());

  ExampleHit hit{};
  WrapperT wrapper1 = hit;
  WrapperT wrapper2 = hit;

  // The operator== compares the underlying pointers
  REQUIRE(wrapper1 == wrapper2);
  // Reassgning to a different entity should make comparisons fail
  wrapper2 = ExampleHit{};
  REQUIRE(wrapper1 != wrapper2);

  // Make sure that the object id functionality work as expected. The wrapped
  // object is in no collection, so it should be the default id.
  REQUIRE(wrapper1.id() == podio::ObjectID{});

  // Create an element in a collection to get one with a non-default object id
  ExampleHitCollection hitColl{};
  hitColl.setID(42);
  wrapper1 = hitColl.create();
  REQUIRE(wrapper1.id() == podio::ObjectID{0, 42});
}

TEST_CASE("InterfaceType from immutable", "[interface-types][basics]") {
  using WrapperT = TypeWithEnergy;

  ExampleHit hit{};
  WrapperT wrapper{hit};
  REQUIRE(wrapper.holds<ExampleHit>());
  REQUIRE_FALSE(wrapper.holds<ExampleCluster>());
  REQUIRE(wrapper.getValue<ExampleHit>() == hit);
  REQUIRE(wrapper == hit);

  ExampleCluster cluster{};
  wrapper = cluster;
  REQUIRE(wrapper.holds<ExampleCluster>());
  REQUIRE(wrapper.getValue<ExampleCluster>() == cluster);
  REQUIRE_THROWS_AS(wrapper.getValue<ExampleHit>(), std::runtime_error);
  REQUIRE(wrapper != hit);
}

TEST_CASE("InterfaceType from mutable", "[interface-types][basics]") {
  using WrapperT = TypeWithEnergy;

  ExampleHit hit{};
  WrapperT wrapper{hit};
  REQUIRE(wrapper.holds<ExampleHit>());
  REQUIRE_FALSE(wrapper.holds<ExampleCluster>());
  REQUIRE(wrapper.getValue<ExampleHit>() == hit);
  REQUIRE(wrapper == hit);
  // Comparison also work against the immutable classes
  ExampleHit immutableHit = hit;
  REQUIRE(wrapper == immutableHit);

  MutableExampleCluster cluster{};
  wrapper = cluster;
  REQUIRE(wrapper.holds<ExampleCluster>());
  REQUIRE(wrapper.getValue<ExampleCluster>() == cluster);
  REQUIRE_THROWS_AS(wrapper.getValue<ExampleHit>(), std::runtime_error);
  REQUIRE(wrapper != hit);
}

TEST_CASE("InterfaceType getters", "[basics][interface-types][code-gen]") {
  MutableExampleCluster cluster{};
  cluster.energy(3.14f);

  TypeWithEnergy interfaceType = cluster;
  REQUIRE(interfaceType.energy() == 3.14f);
}
