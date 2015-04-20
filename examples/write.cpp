// Data model
#include "EventInfoCollection.h"
#include "ExampleHitCollection.h"
#include "ExampleClusterCollection.h"
#include "ExampleReferencingTypeCollection.h"

// STL
#include <iostream>
#include <vector>

// albers specific includes
#include "albers/EventStore.h"
#include "albers/ROOTWriter.h"

int main(){

  std::cout<<"start processing"<<std::endl;

  auto store = albers::EventStore();
  auto writer = albers::ROOTWriter("example.root", &store);

  auto& info     = store.create<EventInfoCollection>("info");
  auto& hits     = store.create<ExampleHitCollection>("hits");
  auto& clusters = store.create<ExampleClusterCollection>("clusters");
  auto& refs     = store.create<ExampleReferencingTypeCollection>("refs");
  auto& refs2    = store.create<ExampleReferencingTypeCollection>("refs2");

  writer.registerForWrite<EventInfoCollection>("info");
  writer.registerForWrite<ExampleHitCollection>("hits");
  writer.registerForWrite<ExampleClusterCollection>("clusters");
  writer.registerForWrite<ExampleReferencingTypeCollection>("refs");
  writer.registerForWrite<ExampleReferencingTypeCollection>("refs2");

  unsigned nevents=10000;

  for(unsigned i=0; i<nevents; ++i) {
    if(i % 1000 == 0) {
      std::cout << "processing event " << i << std::endl;
    }

    // open a scope for the reference counting
    {
      auto item1 = EventInfo();
      item1.Number(i);
      info.push_back(item1);
      auto hit1 = ExampleHit(0.,0.,0.,23.+i);
      auto hit2 = ExampleHit(1.,0.,0.,12.+i);

      hits.push_back(hit1);
      hits.push_back(hit2);

      auto cluster  = ExampleCluster();

      clusters.push_back(cluster);

      cluster.addHits(hit1);
      cluster.addHits(hit2);
      cluster.energy(hit1.energy()+hit2.energy());

      auto ref  = refs.create();
      auto ref2 = refs2.create();
      ref.addClusters(cluster);
      ref.addRefs(ref2);

      auto cyclic = refs.create();
      cyclic.addRefs(cyclic);
    }

    writer.writeEvent();
    store.clearCollections();
  }

  writer.finish();
}
