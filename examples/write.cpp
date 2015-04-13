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
#include "albers/Registry.h"
#include "albers/ROOTWriter.h"

int main(){

  std::cout<<"start processing"<<std::endl;

  albers::Registry   registry;
  albers::EventStore store(&registry);
  albers::ROOTWriter writer("example.root", &registry);

  auto& info     = store.create<EventInfoCollection>("info");
  auto& hits     = store.create<ExampleHitCollection>("hits");
  auto& clusters = store.create<ExampleClusterCollection>("clusters");
  auto& refs     = store.create<ExampleReferencingTypeCollection>("refs");

  writer.registerForWrite<EventInfoCollection>("info");
  writer.registerForWrite<ExampleHitCollection>("hits");
  writer.registerForWrite<ExampleClusterCollection>("clusters");
  writer.registerForWrite<ExampleReferencingTypeCollection>("refs");

  unsigned nevents=10000;

  for(unsigned i=0; i<nevents; ++i) {
    if(i % 1000 == 0) {
      std::cout << "processing event " << i << std::endl;
    }

    // open a scope for the reference counting
    {
      auto item1 = info.create();
      item1.Number(i);
      auto hit1 = hits.create(0.,0.,0.,23.+i);
      auto hit2 = hits.create(1.,0.,0.,12.+i);
      auto cluster  = clusters.create();

      cluster.addHits(hit1);
      cluster.addHits(hit2);
      cluster.energy(hit1.energy()+hit2.energy());

      auto newref  = refs.create();
      newref.addClusters(cluster);
    }

    writer.writeEvent();
    store.clearCollections();
  }

  writer.finish();
}
