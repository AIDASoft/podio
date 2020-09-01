// Data model
#include "datamodel/EventInfoCollection.h"
#include "datamodel/ExampleMCCollection.h"
#include "datamodel/ExampleHitCollection.h"
#include "datamodel/ExampleClusterCollection.h"
#include "datamodel/ExampleReferencingTypeCollection.h"
#include "datamodel/ExampleWithOneRelationCollection.h"
#include "datamodel/ExampleWithVectorMemberCollection.h"
#include "datamodel/ExampleWithComponentCollection.h"
#include "datamodel/ExampleWithNamespaceCollection.h"
#include "datamodel/ExampleWithARelationCollection.h"
#include "datamodel/ExampleWithStringCollection.h"
#include "datamodel/ExampleWithArrayCollection.h"

// STL
#include <iostream>
#include <vector>

// podio specific includes
#include "podio/EventStore.h"
#include "podio/SIOWriter.h"

void write(std::string outfilename) {
  std::cout<<"start processing"<<std::endl;

  auto store = podio::EventStore();
  //auto writer = podio::SIOWriter(outfilename, &store);
  podio::SIOWriter writer(outfilename, &store);

  auto& info       = store.create<EventInfoCollection>("info");
  auto& mcps       = store.create<ExampleMCCollection>("mcparticles");
  auto& hits       = store.create<ExampleHitCollection>("hits");
  auto& clusters   = store.create<ExampleClusterCollection>("clusters");
  auto& refs       = store.create<ExampleReferencingTypeCollection>("refs");
  auto& refs2      = store.create<ExampleReferencingTypeCollection>("refs2");
  auto& comps      = store.create<ExampleWithComponentCollection>("Component");
  auto& oneRels    = store.create<ExampleWithOneRelationCollection>("OneRelation");
  auto& vecs       = store.create<ExampleWithVectorMemberCollection>("WithVectorMember");
  auto& namesps    = store.create<ex42::ExampleWithNamespaceCollection>("WithNamespaceMember");
  auto& namesprels = store.create<ex42::ExampleWithARelationCollection>("WithNamespaceRelation");
  auto& cpytest    = store.create<ex42::ExampleWithARelationCollection>("WithNamespaceRelationCopy");
  auto& strings    = store.create<ExampleWithStringCollection>("strings");
  auto& arrays     = store.create<ExampleWithArrayCollection>("arrays");
  writer.registerForWrite("info");
  writer.registerForWrite("mcparticles");
  writer.registerForWrite("hits");
  writer.registerForWrite("clusters");
  writer.registerForWrite("refs");
  writer.registerForWrite("refs2");
  writer.registerForWrite("Component");
  writer.registerForWrite("OneRelation");
  writer.registerForWrite("WithVectorMember");
  writer.registerForWrite("WithNamespaceMember");
  writer.registerForWrite("WithNamespaceRelation");
  writer.registerForWrite("WithNamespaceRelationCopy");
  writer.registerForWrite("strings");
  writer.registerForWrite("arrays");

  unsigned nevents = 2000;

  for(unsigned i=0; i<nevents; ++i) {
    if(i % 1000 == 0) {
      std::cout << "processing event " << i << std::endl;
    }

    auto item1 = EventInfo();
    item1.Number(i);
    info.push_back(item1);
    auto hit1 = ExampleHit( 0xbad, 0.,0.,0.,23.+i);
    auto hit2 = ExampleHit( 0xcaffee,1.,0.,0.,12.+i);

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

    mcps.push_back( mcp0 ) ;
    mcps.push_back( mcp1 ) ;
    mcps.push_back( mcp2 ) ;
    mcps.push_back( mcp3 ) ;
    mcps.push_back( mcp4 ) ;
    mcps.push_back( mcp5 ) ;
    mcps.push_back( mcp6 ) ;
    mcps.push_back( mcp7 ) ;
    mcps.push_back( mcp8 ) ;
    mcps.push_back( mcp9 ) ;

    // --- add some daughter relations
    auto p = ExampleMC();
    auto d = ExampleMC();

    p = mcps[0] ;
    p.adddaughters( mcps[2] ) ;
    p.adddaughters( mcps[3] ) ;
    p.adddaughters( mcps[4] ) ;
    p.adddaughters( mcps[5] ) ;
    p = mcps[1] ;
    p.adddaughters( mcps[2] ) ;
    p.adddaughters( mcps[3] ) ;
    p.adddaughters( mcps[4] ) ;
    p.adddaughters( mcps[5] ) ;
    p = mcps[2] ;
    p.adddaughters( mcps[6] ) ;
    p.adddaughters( mcps[7] ) ;
    p.adddaughters( mcps[8] ) ;
    p.adddaughters( mcps[9] ) ;
    p = mcps[3] ;
    p.adddaughters( mcps[6] ) ;
    p.adddaughters( mcps[7] ) ;
    p.adddaughters( mcps[8] ) ;
    p.adddaughters( mcps[9] ) ;

    //--- now fix the parent relations
    for( unsigned j=0,N=mcps.size();j<N;++j){
      p = mcps[j] ;
      for(auto it = p.daughters_begin(), end = p.daughters_end() ; it!=end ; ++it ){
  int dIndex = it->getObjectID().index ;
  d = mcps[ dIndex ] ;
  d.addparents( p ) ;
      }
    }
    //-------- print relations for debugging:
    for( auto p : mcps ){
      std::cout << " particle " << p.getObjectID().index << " has daughters: " ;
      for(auto it = p.daughters_begin(), end = p.daughters_end() ; it!=end ; ++it ){
  std::cout << " " << it->getObjectID().index ;
      }
      std::cout << "  and parents: " ;
      for(auto it = p.parents_begin(), end = p.parents_end() ; it!=end ; ++it ){
  std::cout << " " << it->getObjectID().index ;
      }
      std::cout << std::endl ;
    }
    //-------------------------------

    auto cluster  = ExampleCluster();
    auto clu0  = ExampleCluster();
    auto clu1  = ExampleCluster();

    clu0.addHits(hit1);
    clu0.energy(hit1.energy());
    clu1.addHits(hit2);
    clu1.energy(hit2.energy());
    cluster.addHits(hit1);
    cluster.addHits(hit2);
    cluster.energy(hit1.energy()+hit2.energy());
    cluster.addClusters( clu0 ) ;
    cluster.addClusters( clu1 ) ;

    clusters.push_back(clu0);
    clusters.push_back(clu1);
    clusters.push_back(cluster);

    auto ref = ExampleReferencingType();
    refs.push_back(ref);

    auto ref2 = ExampleReferencingType();
    refs2.push_back(ref2);

    ref.addClusters(cluster);
    ref.addRefs(ref2);

    auto comp = ExampleWithComponent();
    comp.component().data.x = 0;
    comp.component().data.y = 1;
    comp.component().data.z = i;
    comps.push_back(comp);

    auto cyclic = ExampleReferencingType();
    cyclic.addRefs(cyclic);
    refs.push_back(cyclic);

    auto oneRel = ExampleWithOneRelation();
    oneRel.cluster(cluster);
    oneRels.push_back(oneRel);

    // write non-filled relation
    auto oneRelEmpty = ExampleWithOneRelation();
    oneRels.push_back(oneRelEmpty);

    auto vec = ExampleWithVectorMember();
    vec.addcount(i);
    vec.addcount(i+10);
    vecs.push_back(vec);
    auto vec1 = ExampleWithVectorMember();
    vec1.addcount(i+1);
    vec1.addcount(i+11);
    vecs.push_back(vec1);

    for (int j = 0; j < 5; j++) {
      auto rel = ex42::ExampleWithARelation();
      rel.number(0.5*j);
      auto exWithNamesp = ex42::ExampleWithNamespace();
      exWithNamesp.data().x = i;
      exWithNamesp.data().y = 1000*i;
      namesps.push_back(exWithNamesp);
      if (j != 3) { // also check for empty relations
        rel.ref(exWithNamesp);
        for (int k = 0; k < 5; k++) {
          auto namesp = ex42::ExampleWithNamespace();
          namesp.x(3*k);
          namesp.data().y = k;
          namesps.push_back(namesp);
          rel.addrefs(namesp);
        }
      }
      namesprels.push_back(rel);
    }
    for (int j = 0; j < namesprels.size(); ++j) {
      cpytest.push_back(namesprels.at(j).clone());
    }

    auto string = ExampleWithString("SomeString");
    strings.push_back(string);

    std::array<int, 4> arrayTest = {0, 0, 2, 3};
    std::array<int, 4> arrayTest2 = {4, 4, 2 * static_cast<int>(i)};
    NotSoSimpleStruct a;
    a.data.p = arrayTest2;
    ex2::NamespaceStruct nstruct;
    nstruct.x = static_cast<int>(i);
    std::array<ex2::NamespaceStruct, 4> structArrayTest = {nstruct, nstruct, nstruct, nstruct};
    auto array = ExampleWithArray(a, arrayTest, arrayTest, arrayTest, arrayTest, structArrayTest);
    array.myArray(1, i);
    array.arrayStruct(a);
    arrays.push_back(array);

    writer.writeEvent();
    store.clearCollections();
  }

  writer.finish();
}


int main(int argc, char* argv[]){
  write("example.sio");
  write("example1.sio");


}
