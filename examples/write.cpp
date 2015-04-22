// Data model
#include "EventInfoCollection.h"
#include "ExampleHitCollection.h"
#include "ExampleClusterCollection.h"
#include "ExampleReferencingTypeCollection.h"
#include "ExampleWithOneRelationCollection.h"
#include "ExampleWithVectorMemberCollection.h"
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
  auto& oneRels  = store.create<ExampleWithOneRelationCollection>("OneRelation");
  auto& vecs     = store.create<ExampleWithVectorMemberCollection>("WithVectorMember");
  writer.registerForWrite<EventInfoCollection>("info");
  writer.registerForWrite<ExampleHitCollection>("hits");
  writer.registerForWrite<ExampleClusterCollection>("clusters");
  writer.registerForWrite<ExampleReferencingTypeCollection>("refs");
  writer.registerForWrite<ExampleReferencingTypeCollection>("refs2");
  writer.registerForWrite<ExampleWithOneRelationCollection>("OneRelation");
  writer.registerForWrite<ExampleWithVectorMemberCollection>("WithVectorMember");

  unsigned nevents=10000;

  for(unsigned i=0; i<nevents; ++i) {
    if(i % 1000 == 0) {
      std::cout << "processing event " << i << std::endl;
    }

    auto item1 = EventInfo();
    item1.Number(i);
    info.push_back(item1);
    auto hit1 = ExampleHit(0.,0.,0.,23.+i);
    auto hit2 = ExampleHit(1.,0.,0.,12.+i);

    hits.push_back(hit1);
    hits.push_back(hit2);

    auto cluster  = ExampleCluster();
    cluster.addHits(hit1);
    cluster.addHits(hit2);
    cluster.energy(hit1.energy()+hit2.energy());

    clusters.push_back(cluster);

    auto ref = ExampleReferencingType();
    refs.push_back(ref);

    auto ref2 = ExampleReferencingType();
    refs2.push_back(ref2);

    ref.addClusters(cluster);
    ref.addRefs(ref2);

    auto cyclic = ExampleReferencingType();
    cyclic.addRefs(cyclic);
    refs.push_back(cyclic);

    auto oneRel = ExampleWithOneRelation();
    oneRel.cluster(cluster);
    oneRels.push_back(oneRel);

    auto vec = ExampleWithVectorMember();
    vec.count().push_back(23);
    vec.count().push_back(24);
    vecs.push_back(vec);

    writer.writeEvent();
    store.clearCollections();
  }

  writer.finish();
}
