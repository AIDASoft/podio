// Data model
#include "datamodel/EventInfoCollection.h"
#include "datamodel/ExampleClusterCollection.h"
#include "datamodel/ExampleHitCollection.h"
#include "datamodel/ExampleMCCollection.h"
#include "datamodel/ExampleReferencingTypeCollection.h"
#include "datamodel/ExampleWithARelationCollection.h"
#include "datamodel/ExampleWithComponentCollection.h"
#include "datamodel/ExampleWithNamespaceCollection.h"
#include "datamodel/ExampleWithOneRelationCollection.h"
#include "datamodel/ExampleWithStringCollection.h"
#include "datamodel/ExampleWithVectorMemberCollection.h"

// STL
#include <iostream>
#include <vector>

// podio specific includes
#include "podio/ASCIIWriter.h"
#include "podio/EventStore.h"

int main() {

  std::cout << "start processing" << std::endl;

  auto store = podio::EventStore();
  auto writer = podio::ASCIIWriter("example.txt", &store);

  auto& info = store.create<EventInfoCollection>("info");
  auto& mcps = store.create<ExampleMCCollection>("mcparticles");
  auto& hits = store.create<ExampleHitCollection>("hits");
  auto& clusters = store.create<ExampleClusterCollection>("clusters");
  auto& refs = store.create<ExampleReferencingTypeCollection>("refs");
  auto& refs2 = store.create<ExampleReferencingTypeCollection>("refs2");
  auto& comps = store.create<ExampleWithComponentCollection>("Component");
  auto& oneRels = store.create<ExampleWithOneRelationCollection>("OneRelation");
  auto& vecs = store.create<ExampleWithVectorMemberCollection>("WithVectorMember");
  auto& namesps = store.create<ex42::ExampleWithNamespaceCollection>("WithNamespaceMember");
  auto& namesprels = store.create<ex42::ExampleWithARelationCollection>("WithNamespaceRelation");
  auto& strings = store.create<ExampleWithStringCollection>("strings");
  writer.registerForWrite<EventInfoCollection>("info");
  writer.registerForWrite<ExampleMCCollection>("mcparticles");
  writer.registerForWrite<ExampleHitCollection>("hits");
  writer.registerForWrite<ExampleClusterCollection>("clusters");
  writer.registerForWrite<ExampleReferencingTypeCollection>("refs");
  writer.registerForWrite<ExampleReferencingTypeCollection>("refs2");
  writer.registerForWrite<ExampleWithComponentCollection>("Component");
  writer.registerForWrite<ExampleWithOneRelationCollection>("OneRelation");
  writer.registerForWrite<ExampleWithVectorMemberCollection>("WithVectorMember");
  writer.registerForWrite<ex42::ExampleWithNamespaceCollection>("WithNamespaceMember");
  writer.registerForWrite<ex42::ExampleWithARelationCollection>("WithNamespaceRelation");
  writer.registerForWrite<ExampleWithStringCollection>("strings");

  unsigned nevents = 1; // 2000;

  for (unsigned i = 0; i < nevents; ++i) {
    if (i % 1000 == 0) {
      std::cout << "processing event " << i << std::endl;
    }

    auto item1 = MutableEventInfo();
    item1.Number(i);
    info.push_back(item1);
    auto hit1 = ExampleHit(0xbad, 0., 0., 0., 23. + i);
    auto hit2 = ExampleHit(0xcaffee, 1., 0., 0., 12. + i);

    hits.push_back(hit1);
    hits.push_back(hit2);

    // ---- add some MC particles ----
    auto mcp0 = ExampleMC();
    auto mcp1 = ExampleMC();
    auto mcp2 = ExampleMC();
    auto mcp3 = ExampleMC();
    auto mcp4 = ExampleMC();
    auto mcp5 = ExampleMC();
    auto mcp6 = ExampleMC();
    auto mcp7 = ExampleMC();
    auto mcp8 = ExampleMC();
    auto mcp9 = ExampleMC();

    mcps.push_back(mcp0);
    mcps.push_back(mcp1);
    mcps.push_back(mcp2);
    mcps.push_back(mcp3);
    mcps.push_back(mcp4);
    mcps.push_back(mcp5);
    mcps.push_back(mcp6);
    mcps.push_back(mcp7);
    mcps.push_back(mcp8);
    mcps.push_back(mcp9);

    // --- add some daughter relations

    auto p = mcps[0];
    p.adddaughters(mcps[2]);
    p.adddaughters(mcps[3]);
    p.adddaughters(mcps[4]);
    p.adddaughters(mcps[5]);
    p = mcps[1];
    p.adddaughters(mcps[2]);
    p.adddaughters(mcps[3]);
    p.adddaughters(mcps[4]);
    p.adddaughters(mcps[5]);
    p = mcps[2];
    p.adddaughters(mcps[6]);
    p.adddaughters(mcps[7]);
    p.adddaughters(mcps[8]);
    p.adddaughters(mcps[9]);
    p = mcps[3];
    p.adddaughters(mcps[6]);
    p.adddaughters(mcps[7]);
    p.adddaughters(mcps[8]);
    p.adddaughters(mcps[9]);

    //--- now fix the parent relations
    for (unsigned j = 0, N = mcps.size(); j < N; ++j) {
      p = mcps[j];
      for (auto it = p.daughters_begin(), end = p.daughters_end(); it != end; ++it) {
        int dIndex = it->getObjectID().index;
        auto d = mcps[dIndex];
        d.addparents(p);
      }
    }
    //-------- print relations for debugging:
    for (auto _p : mcps) {
      std::cout << " particle " << _p.getObjectID().index << " has daughters: ";
      for (auto it = _p.daughters_begin(), end = _p.daughters_end(); it != end; ++it) {
        std::cout << " " << it->getObjectID().index;
      }
      std::cout << "  and parents: ";
      for (auto it = _p.parents_begin(), end = _p.parents_end(); it != end; ++it) {
        std::cout << " " << it->getObjectID().index;
      }
      std::cout << std::endl;
    }
    //-------------------------------

    auto cluster = MutableExampleCluster();
    auto clu0 = MutableExampleCluster();
    auto clu1 = MutableExampleCluster();

    clu0.addHits(hit1);
    clu0.energy(hit1.energy());
    clu1.addHits(hit2);
    clu1.energy(hit2.energy());
    cluster.addHits(hit1);
    cluster.addHits(hit2);
    cluster.energy(hit1.energy() + hit2.energy());
    cluster.addClusters(clu0);
    cluster.addClusters(clu1);

    clusters.push_back(clu0);
    clusters.push_back(clu1);
    clusters.push_back(cluster);

    auto ref = MutableExampleReferencingType();
    refs.push_back(ref);

    auto ref2 = ExampleReferencingType();
    refs2.push_back(ref2);

    ref.addClusters(cluster);
    ref.addRefs(ref2);

    auto comp = MutableExampleWithComponent();
    comp.component().data.x = 0;
    comp.component().data.y = 1;
    comp.component().data.z = i;
    comps.push_back(comp);

    auto cyclic = MutableExampleReferencingType();
    cyclic.addRefs(cyclic);
    refs.push_back(cyclic);

    auto oneRel = MutableExampleWithOneRelation();
    oneRel.cluster(cluster);
    oneRels.push_back(oneRel);

    // write non-filled relation
    auto oneRelEmpty = ExampleWithOneRelation();
    oneRels.push_back(oneRelEmpty);

    auto vec = MutableExampleWithVectorMember();
    vec.addcount(23);
    vec.addcount(24);
    vecs.push_back(vec);

    auto namesp = ex42::MutableExampleWithNamespace();
    namesp.component().x = 1;
    namesp.component().y = i;
    namesps.push_back(namesp);

    auto rel = ex42::MutableExampleWithARelation();
    rel.ref(namesp);
    namesprels.push_back(rel);

    auto string = ExampleWithString("SomeString");
    strings.push_back(string);

    writer.writeEvent();
    store.clearCollections();
  }

  writer.finish();
}
