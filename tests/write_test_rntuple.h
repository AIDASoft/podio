//TODO: remove after issue with rntuple and int16_t is fixed

#ifndef PODIO_TESTS_WRITE_TEST_H_
#define PODIO_TESTS_WRITE_TEST_H_

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
//#include "datamodel/ExampleWithFixedWidthIntegersCollection.h"

#include "podio/UserDataCollection.h"
#include "podio/EventStore.h"

// STL
#include <iostream>
#include <vector>
#include <sstream>
#include <limits>

template<class WriterT>
void write(podio::EventStore& store, WriterT& writer) {
  std::cout<<"start processing"<<std::endl;

  auto& info       = store.create<EventInfoCollection>("info");
  auto& mcps       = store.create<ExampleMCCollection>("mcparticles");
  auto& moreMCs    = store.create<ExampleMCCollection>("moreMCs");
  auto& mcpsRefs   = store.create<ExampleMCCollection>("mcParticleRefs");
  mcpsRefs.setSubsetCollection();
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
//  auto& fixedWidthInts = store.create<ExampleWithFixedWidthIntegersCollection>("fixedWidthInts");
  auto& usrInts = store.create<podio::UserDataCollection<uint64_t> >("userInts");
  auto& usrDoubles = store.create<podio::UserDataCollection<double> >("userDoubles");

  writer.registerForWrite("info");
  writer.registerForWrite("mcparticles");
  writer.registerForWrite("moreMCs");
  writer.registerForWrite("mcParticleRefs");
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
  //writer.registerForWrite("fixedWidthInts");
  writer.registerForWrite("userInts");
  writer.registerForWrite("userDoubles");

  unsigned nevents = 2000;

  for(unsigned i=0; i<nevents; ++i) {
    if(i % 1000 == 0) {
      std::cout << "processing event " << i << std::endl;
    }

    auto item1 = EventInfo();
    item1.Number(i);
    info.push_back(item1);

    auto& evtMD = store.getEventMetaData() ;
    evtMD.setValue( "UserEventWeight" , (float) 100.*i ) ;
    std::stringstream ss ; ss << " event_number_" << i ;
    evtMD.setValue( "UserEventName" , ss.str() ) ;


    auto& colMD = store.getCollectionMetaData( hits.getID() );
    colMD.setValue("CellIDEncodingString","system:8,barrel:3,layer:6,slice:5,x:-16,y:-16");

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

    auto mcp = mcps[0] ;
    mcp.adddaughters( mcps[2] ) ;
    mcp.adddaughters( mcps[3] ) ;
    mcp.adddaughters( mcps[4] ) ;
    mcp.adddaughters( mcps[5] ) ;
    mcp = mcps[1] ;
    mcp.adddaughters( mcps[2] ) ;
    mcp.adddaughters( mcps[3] ) ;
    mcp.adddaughters( mcps[4] ) ;
    mcp.adddaughters( mcps[5] ) ;
    mcp = mcps[2] ;
    mcp.adddaughters( mcps[6] ) ;
    mcp.adddaughters( mcps[7] ) ;
    mcp.adddaughters( mcps[8] ) ;
    mcp.adddaughters( mcps[9] ) ;
    mcp = mcps[3] ;
    mcp.adddaughters( mcps[6] ) ;
    mcp.adddaughters( mcps[7] ) ;
    mcp.adddaughters( mcps[8] ) ;
    mcp.adddaughters( mcps[9] ) ;

    //--- now fix the parent relations
    // use a range-based for loop here to see if we get mutable objects from the
    // begin/end iterators
    for (auto mc : mcps) {
      for(auto p : mc.daughters()) {
        int dIndex = p.getObjectID().index ;
        auto d = mcps[ dIndex ] ;
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

      // make sure that this does not crash when we do it on a ConstExampleMC
      ConstExampleMC constP{p};
      std::cout << "The const particle still has the same relations: daughters: ";
      for (auto it = constP.daughters_begin(); it != constP.daughters_end(); ++it) {
        std::cout << " " << it->getObjectID().index;
      }
      std::cout << " and parents: ";
      for (auto it = constP.parents_begin(); it != constP.parents_end(); ++it) {
        std::cout << " " << it->getObjectID().index;
      }

    }
    //-------------------------------

    // ----------------- create a second MC collection -----------------
    // Can use it to test subset collections that store elements from multiple
    // collections
    for (const auto&& mc : mcps) {
      moreMCs.push_back(mc.clone());
    }

    // ----------------- add all "odd" mc particles into a subset collection
    for (auto p : mcps) {
      if (p.id() % 2) {
        mcpsRefs.push_back(p);
      }
    }
    // ----------------- add the "even" counterparts from a different collection
    for (auto p : moreMCs) {
      if (p.id() % 2 == 0) {
        mcpsRefs.push_back(p);
      }
    }

    if (mcpsRefs.size() != mcps.size()) {
      throw std::runtime_error("The mcParticleRefs collection should now contain as many elements as the mcparticles collection");
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
      exWithNamesp.component().x = i;
      exWithNamesp.component().y = 1000*i;
      namesps.push_back(exWithNamesp);
      if (j != 3) { // also check for empty relations
        rel.ref(exWithNamesp);
        for (int k = 0; k < 5; k++) {
          auto namesp = ex42::ExampleWithNamespace();
          namesp.x(3*k);
          namesp.component().y = k;
          namesps.push_back(namesp);
          rel.addrefs(namesp);
        }
      }
      namesprels.push_back(rel);
    }
    for (size_t j = 0; j < namesprels.size(); ++j) {
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

    //auto maxValues = fixedWidthInts.create();
    //maxValues.fixedI16(std::numeric_limits<std::int16_t>::max()); // 2^(16 - 1) - 1 == 32767
    //maxValues.fixedU32(std::numeric_limits<std::uint32_t>::max()); // 2^32 - 1 == 4294967295
    //maxValues.fixedU64(std::numeric_limits<std::uint64_t>::max()); // 2^64 - 1 == 18446744073709551615
    //auto& maxComp = maxValues.fixedWidthStruct();
    //maxComp.fixedUnsigned16 = std::numeric_limits<std::uint16_t>::max(); // 2^16 - 1 == 65535
    //maxComp.fixedInteger64 = std::numeric_limits<std::int64_t>::max(); // 2^(64 -1) - 1 == 9223372036854775807
    //maxComp.fixedInteger32 = std::numeric_limits<std::int32_t>::max(); // 2^(32 - 1) - 1 == 2147483647

    //auto minValues = fixedWidthInts.create();
    //minValues.fixedI16(std::numeric_limits<std::int16_t>::min()); // -2^(16 - 1) == -32768
    //minValues.fixedU32(std::numeric_limits<std::uint32_t>::min()); // 0
    //minValues.fixedU64(std::numeric_limits<std::uint64_t>::min()); // 0
    //auto& minComp = minValues.fixedWidthStruct();
    //minComp.fixedUnsigned16 = std::numeric_limits<std::uint16_t>::min(); // 0
    //minComp.fixedInteger64 = std::numeric_limits<std::int64_t>::min(); // -2^(64 - 1) == -9223372036854775808
    //minComp.fixedInteger32 = std::numeric_limits<std::int32_t>::min(); // -2^(32 - 1) == -2147483648

    //auto arbValues = fixedWidthInts.create();
    //arbValues.fixedI16(-12345);
    //arbValues.fixedU32(1234567890);
    //arbValues.fixedU64(1234567890123456789);
    //auto& arbComp = arbValues.fixedWidthStruct();
    //arbComp.fixedUnsigned16 = 12345;
    //arbComp.fixedInteger32 = -1234567890;
    //arbComp.fixedInteger64 = -1234567890123456789ll;


    // add some plain ints as user data
    auto& uivec = usrInts ;
    uivec.resize( i + 1 ) ;
    int myInt = 0 ;
    for( auto& iu : uivec ){
      iu = myInt++  ;
    }
    // and some user double values
    unsigned nd = 100 ;
    usrDoubles.resize( nd ) ;
    for(unsigned id=0 ; id<nd ; ++id){
      usrDoubles[id] = 42. ;
    }


    writer.writeEvent();
    store.clearCollections();
  }
  std::cout << std::endl;

  writer.finish();
}


#endif // __WRITE_TEST_H_
