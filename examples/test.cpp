// STL
#include <iostream>
#include <vector>

// albers specific includes
#include "albers/EventStore.h"

// Test data types
#include "EventInfoCollection.h"
#include "ExampleClusterCollection.h"
#include "ExampleHitCollection.h"

void test_autodelete() {
  std::cout << "*** Test autodelete ***" << std::endl;
  auto store = albers::EventStore();
  auto hit1 = EventInfo();
  auto hit2 = EventInfo();
  auto hit3 = EventInfo();
  auto& coll  = store.create<EventInfoCollection>("info");
  coll.push_back(hit1);
  std::cout << "Should delete one object now:" << std::endl;
  hit3 = hit2;
  std::cout << "Should delete one object now:" << std::endl;
}

void test_basic(){
  std::cout << "*** Test basics ***" << std::endl;
  auto store = albers::EventStore();
  // Adding
  auto& collection = store.create<ExampleHitCollection>("name");
  auto hit1 = collection.create(0.,0.,0.,0.); //initialize w/ value
  auto hit2 = collection.create(); //default initialize
  hit2.energy(12.5);
  // Retrieving
  const ExampleHitCollection* coll2(nullptr);
  bool success = store.get("name",coll2);
  std::cout << "    Success: " << success << std::endl;
}

void test_clearing() {
  std::cout << "*** Test clearing ***" << std::endl;
  bool success = true;
  auto store = albers::EventStore();
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
  std::cout << "    Success: " << success << std::endl;
}

void test_cloning() {
  std::cout << "*** Test cloning ***" << std::endl;
  std::cout << "    TODO " << std::endl;

}

void test_invalid_refs() {
  std::cout << "*** Test invalid refs ***" << std::endl;
  bool success = false;
  auto store = albers::EventStore();
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
  std::cout << "    Success: " << success << std::endl;
}

void test_looping(){
  std::cout << "*** Test looping ***" << std::endl;
  bool success = true;
  auto store = albers::EventStore();
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
  std::cout << "    Success: " << success << std::endl;
}

void test_notebook() {
  bool success = true;
  std::cout << "*** Test notebook ***" << std::endl;
  auto store = albers::EventStore();
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
  std::cout << "    Success: " << success << std::endl;
}

void test_POD(){
  std::cout << "*** Test PODness ***" << std::endl;
  bool success = true;
  if (std::is_pod<ExampleClusterData>() != true) success = false;
  std::cout << "    Success: " << success << std::endl;
}

void test_referencing(){
  std::cout << "*** Test referencing ***" << std::endl;
  bool success = true;
  auto store = albers::EventStore();
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
  std::cout << "    Success: " << success << std::endl;
}

void test_write_buffer() {
  std::cout << "*** Test write buffer ***" << std::endl;
  bool success = true;
  auto store = albers::EventStore();
  auto& coll  = store.create<ExampleHitCollection>("data");
  auto hit1 = coll.create(0.,0.,0.,0.);
  auto hit2 = coll.create(1.,1.,1.,1.);
  auto& clusters  = store.create<ExampleClusterCollection>("clusters");
  auto cluster  = clusters.create();
  clusters.prepareForWrite();
  std::cout << "    Success: " << success << std::endl;
}

int main(){

  test_autodelete();
  test_basic();
  test_clearing();
  test_cloning();
  test_invalid_refs();
  test_looping();
  test_notebook();
  test_POD();
  test_referencing();
  test_write_buffer();

}
