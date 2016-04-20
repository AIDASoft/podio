// STL
#include <iostream>
#include <vector>

// catch
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

// podio specific includes
#include "podio/EventStore.h"

// Test data types
#include "EventInfoCollection.h"
#include "ExampleClusterCollection.h"
#include "ExampleForCyclicDependency1Collection.h"
#include "ExampleForCyclicDependency2Collection.h"
#include "ExampleHitCollection.h"
#include "ExampleWithComponent.h"
#include "ExampleWithOneRelation.h"
#include "ExampleWithOneRelationCollection.h"

TEST_CASE("AutoDelete") {
  auto store = podio::EventStore();
  auto hit1 = EventInfo();
  auto hit2 = EventInfo();
  auto hit3 = EventInfo();
  auto& coll  = store.create<EventInfoCollection>("info");
  coll.push_back(hit1);
  hit3 = hit2;
}

TEST_CASE("Basics") {
  auto store = podio::EventStore();
  // Adding
  auto& collection = store.create<ExampleHitCollection>("name");
  auto hit1 = collection.create(0.,0.,0.,0.); //initialize w/ value
  auto hit2 = collection.create(); //default initialize
  hit2.energy(12.5);
  // Retrieving
  const ExampleHitCollection* coll2(nullptr);
  bool success = store.get("name",coll2);
  const ExampleHitCollection* coll3(nullptr);
  if (store.get("wrongName",coll3) != false) success = false;
  REQUIRE(success);
}

TEST_CASE("Clearing"){
  bool success = true;
  auto store = podio::EventStore();
  auto& hits  = store.create<ExampleHitCollection>("hits");
  auto& clusters  = store.create<ExampleClusterCollection>("clusters");
  auto& oneRels    = store.create<ExampleWithOneRelationCollection>("OneRelation");
  auto nevents = unsigned(1000);
  for(unsigned i=0; i<nevents; ++i){
    hits.clear();
    clusters.clear();
    auto hit1 = hits.create();
    auto hit2 = ExampleHit();
    hit1.energy(double(i));
    auto cluster  = clusters.create();
    cluster.addHits(hit1);
    cluster.addHits(hit2);
    hits.push_back(hit2);
    auto oneRel = ExampleWithOneRelation();
    oneRel.cluster(cluster);
    oneRel.cluster(cluster);
    oneRels.push_back(oneRel);
  }
  hits.clear();
  if (hits.size() != 0 ) success = false;
  REQUIRE(success);
}

TEST_CASE("Cloning"){
  bool success = true;
  auto hit = ExampleHit();
  hit.energy(30);
  auto hit2 = hit.clone();
  hit2.energy(20);
  if (hit.energy() == hit2.energy()) success = false;
  auto cluster  = ExampleCluster();
  cluster.addHits(hit);
  auto cluster2 = cluster.clone();
  cluster.addHits(hit2);
  REQUIRE(success);
}

TEST_CASE("Component"){
  auto info = ExampleWithComponent();
  info.component().data.x = 3;
  REQUIRE(3 == info.component().data.x);
}

TEST_CASE("Cyclic"){
  auto start = ExampleForCyclicDependency1();
  auto isAvailable = start.ref().isAvailable();
  REQUIRE_FALSE(isAvailable);
  auto end = ExampleForCyclicDependency2();
  start.ref(end);
  isAvailable = start.ref().isAvailable();
  REQUIRE(isAvailable);
  end.ref(start);
  REQUIRE(start == end.ref());
  auto end_eq = start.ref();
  auto start_eq = end_eq.ref();
  REQUIRE(start == start_eq);
  REQUIRE(start == start.ref().ref());
}

TEST_CASE("Invalid_refs") {
  bool success = false;
  auto store = podio::EventStore();
  auto& hits  = store.create<ExampleHitCollection>("hits");
  auto hit1 = hits.create(0.,0.,0.,0.);
  auto hit2 = ExampleHit();
  auto& clusters  = store.create<ExampleClusterCollection>("clusters");
  auto  cluster  = clusters.create();
  cluster.addHits(hit1);
  cluster.addHits(hit2);
  try {
    clusters.prepareForWrite(); //should fail!
  } catch (std::runtime_error){
    success = true;
  }
  REQUIRE(success);
}

TEST_CASE("Looping") {
  bool success = true;
  auto store = podio::EventStore();
  auto& coll  = store.create<ExampleHitCollection>("name");
  auto hit1 = coll.create(0.,0.,0.,0.);
  auto hit2 = coll.create(1.,1.,1.,1.);
  for(auto i = coll.begin(), end = coll.end(); i != end; ++i) {
    auto energy = i->energy();
  }
  for(int i = 0, end = coll.size(); i != end; ++i) {
    auto energy = coll[i].energy();
  }
  if ((coll[0].energy() != 0) || (coll[1].energy() != 1)) success = false;
  REQUIRE(success);
}

TEST_CASE("Notebook") {
  bool success = true;
  auto store = podio::EventStore();
  auto& hits  = store.create<ExampleHitCollection>("hits");
  for(unsigned i=0; i<12; ++i){
    auto hit = hits.create(0.,0.,0.,double(i));
  }
  auto energies = hits.energy<10>();
  int index = 0;
  for (auto energy : energies){
    if(double(index) != energy) success = false;
    ++index;
  }
  REQUIRE(success);
}

TEST_CASE("OneToOneRelations") {
  bool success = true;
  auto cluster = ExampleCluster();
  auto rel = ExampleWithOneRelation();
  rel.cluster(cluster);
  REQUIRE(success);
}

TEST_CASE("Podness") {
  REQUIRE(std::is_pod<ExampleClusterData>());
  REQUIRE(std::is_pod<ExampleHitData>());
  REQUIRE(std::is_pod<ExampleWithOneRelationData>());
  // just to be sure the test does what it is supposed to do
  REQUIRE_FALSE(std::is_pod<ExampleClusterObj>());
}

TEST_CASE("Referencing") {
  bool success = true;
  auto store = podio::EventStore();
  auto& hits  = store.create<ExampleHitCollection>("hits");
  auto hit1 = hits.create(0.,0.,0.,0.);
  auto hit2 = hits.create(1.,1.,1.,1.);
  auto& clusters  = store.create<ExampleClusterCollection>("clusters");
  auto  cluster  = clusters.create();
  cluster.addHits(hit1);
  cluster.addHits(hit2);
  int index = 0;
  for (auto i = cluster.Hits_begin(), end = cluster.Hits_end(); i!=end; ++i){
    if( i->energy() != index) success = false;
    ++index;
  }
  REQUIRE(success);
}

TEST_CASE("write_buffer") {
  bool success = true;
  auto store = podio::EventStore();
  auto& coll  = store.create<ExampleHitCollection>("data");
  auto hit1 = coll.create(0.,0.,0.,0.);
  auto hit2 = coll.create(1.,1.,1.,1.);
  auto& clusters  = store.create<ExampleClusterCollection>("clusters");
  auto cluster  = clusters.create();
  clusters.prepareForWrite();
  REQUIRE(success);
  auto& ref_coll  = store.create<ExampleWithOneRelationCollection>("onerel");
  auto withRef = ref_coll.create();
  ref_coll.prepareForWrite();

}

/*
TEST_CASE("Arrays") {
  auto obj = ExampleWithArray();
  obj.array({1,2,3});
  REQUIRE( obj.array()[0] == 1);
}
*/

TEST_CASE("Extracode") {
  auto ev = EventInfo();
  ev.setNumber(42) ;
  REQUIRE(ev.getNumber() == 42);

  int ia[3] = { 1 , 2 , 3 } ;
  auto simple = SimpleStruct( ia ) ;
  REQUIRE( simple.x == 1 );
  REQUIRE( simple.y == 2 );
  REQUIRE( simple.z == 3 );


}

TEST_CASE("Equality") {
  auto cluster = ExampleCluster();
  auto rel = ExampleWithOneRelation();
  rel.cluster(cluster);
  auto returned_cluster = rel.cluster();
  REQUIRE(cluster == returned_cluster);
  REQUIRE(returned_cluster == cluster);
}
