// STL
#include <iostream>
#include <vector>

// gtest
#include "gtest/gtest.h"

// podio specific includes
#include "podio/EventStore.h"

// Test data types
#include "EventInfoCollection.h"
#include "ExampleClusterCollection.h"
#include "ExampleHitCollection.h"
#include "ExampleWithOneRelation.h"
#include "ExampleWithOneRelationCollection.h"

TEST(podio, AutoDelete) {
  auto store = podio::EventStore();
  auto hit1 = EventInfo();
  auto hit2 = EventInfo();
  auto hit3 = EventInfo();
  auto& coll  = store.create<EventInfoCollection>("info");
  coll.push_back(hit1);
  hit3 = hit2;
}

TEST(podio, Basics) {
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
  EXPECT_EQ(true, success);
}

TEST(podio, Clearing){
  bool success = true;
  auto store = podio::EventStore();
  auto& hits  = store.create<ExampleHitCollection>("hits");
  auto& clusters  = store.create<ExampleClusterCollection>("clusters");
  auto nevents = unsigned(1000);
  for(unsigned i=0; i<nevents; ++i){
    hits.clear();
    clusters.clear();
    auto hit1 = hits.create();
    hit1.energy(double(i));
    auto hit2 = hits.create();
    auto cluster  = clusters.create();
    cluster.addHits(hit1);
  }
  hits.clear();
  if (hits.size() != 0 ) success = false;
  EXPECT_EQ(true, success);
}

TEST(podio, cloning){
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
  EXPECT_EQ(true, success);
}

TEST(podio, invalid_refs) {
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
  EXPECT_EQ(true, success);
}

TEST(podio,looping) {
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
  EXPECT_EQ(true, success);
}

TEST(podio,notebook) {
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
  EXPECT_EQ(true, success);
}

TEST(podio,OneToOneRelations) {
  bool success = true;
  auto cluster = ExampleCluster();
  auto rel = ExampleWithOneRelation();
  rel.cluster(cluster);
  EXPECT_EQ(true, success);
}

TEST(podio,podness) {
  EXPECT_EQ(true, std::is_pod<ExampleClusterData>());
  EXPECT_EQ(true, std::is_pod<ExampleHitData>());
  EXPECT_EQ(true, std::is_pod<ExampleWithOneRelationData>());
  // just to be sure the test does what it is supposed to do
  EXPECT_EQ(false, std::is_pod<ExampleClusterObj>());
}

TEST(podio,referencing) {
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
  EXPECT_EQ(true, success);
}

TEST(podio, write_buffer) {
  bool success = true;
  auto store = podio::EventStore();
  auto& coll  = store.create<ExampleHitCollection>("data");
  auto hit1 = coll.create(0.,0.,0.,0.);
  auto hit2 = coll.create(1.,1.,1.,1.);
  auto& clusters  = store.create<ExampleClusterCollection>("clusters");
  auto cluster  = clusters.create();
  clusters.prepareForWrite();
  EXPECT_EQ(true, success);
}

TEST(podio, equality) {
  auto cluster = ExampleCluster();
  auto rel = ExampleWithOneRelation();
  rel.cluster(cluster);
  auto returned_cluster = rel.cluster();
  EXPECT_EQ(cluster,returned_cluster);
  EXPECT_EQ(returned_cluster,cluster);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
