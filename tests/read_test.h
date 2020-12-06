#ifndef PODIO_TESTS_READ_TEST_H_
#define PODIO_TESTS_READ_TEST_H_
// test data model
#include "datamodel/ExampleHitCollection.h"
#include "datamodel/ExampleClusterCollection.h"
#include "datamodel/ExampleMCCollection.h"
#include "datamodel/ExampleReferencingTypeCollection.h"
#include "datamodel/ExampleWithOneRelationCollection.h"
#include "datamodel/ExampleWithVectorMemberCollection.h"
#include "datamodel/ExampleWithComponentCollection.h"
#include "datamodel/ExampleWithARelationCollection.h"
#include "datamodel/ExampleWithStringCollection.h"
#include "datamodel/ExampleWithNamespace.h"
#include "datamodel/ExampleWithArrayCollection.h"

// podio specific includes
#include "podio/EventStore.h"
#include "podio/IReader.h"

// STL
#include <vector>
#include <iostream>
#include <exception>
#include <cassert>
#include <sstream>


void processEvent(podio::EventStore& store, bool verboser, unsigned eventNum) {

  auto evtMD = store.getEventMetaData() ;
  float evtWeight = evtMD.getFloatVal( "UserEventWeight" ) ;
  if( evtWeight != (float) 100.*eventNum ){
    std::cout << " read UserEventWeight: " << evtWeight << " - expected : " << (float) 100.*eventNum << std::endl ;
    throw std::runtime_error("Couldn't read event meta data parameters 'UserEventWeight'");
  }
  std::stringstream ss ; ss << " event_number_" << eventNum ;
  const auto& evtName = evtMD.getStringVal( "UserEventName" ) ;
  if( evtName != ss.str() ){
    std::cout << " read UserEventName: " << evtName << " - expected : " << ss.str() << std::endl ;
    throw std::runtime_error("Couldn't read event meta data parameters 'UserEventName'");
  }


  try {
    auto& failing = store.get<ExampleClusterCollection>("notthere");
  } catch(const std::runtime_error& err) {
    if (std::string(err.what()) != "No collection \'notthere\' is present in the EventStore") {
      throw std::runtime_error("Trying to get non present collection \'notthere' should throw an exception");
    }
  }

  auto& strings = store.get<ExampleWithStringCollection>("strings");
  if(strings.isValid()){
    auto string = strings[0];
    if (string.theString() != "SomeString") {
      throw std::runtime_error("Couldn't read string properly");
    }
  } else {
    throw std::runtime_error("Collection 'strings' should be present.");
  }

  // read collection meta data
  auto& hits = store.get<ExampleHitCollection>("hits");
  auto colMD = store.getCollectionMetaData( hits.getID() );
  const auto& es = colMD.getStringVal("CellIDEncodingString") ;
  if( es != std::string("system:8,barrel:3,layer:6,slice:5,x:-16,y:-16") ){
    std::cout << " meta data from collection 'hits' with id = " <<  hits.getID()
              << " read CellIDEncodingString: " << es << " - expected : system:8,barrel:3,layer:6,slice:5,x:-16,y:-16"
              << std::endl ;
    throw std::runtime_error("Couldn't read event meta data parameters 'CellIDEncodingString'");
  }


  auto& clusters = store.get<ExampleClusterCollection>("clusters");
  if(clusters.isValid()){
    auto cluster = clusters[0];
    for (auto i = cluster.Hits_begin(), end = cluster.Hits_end(); i!=end; ++i){
      std::cout << "  Referenced hit has an energy of " << i->energy() << std::endl;
    }
  } else {
    throw std::runtime_error("Collection 'clusters' should be present");
  }


  auto& mcps =  store.get<ExampleMCCollection>("mcparticles");
  if( mcps.isValid() ){
    // check that we can retrieve the correct parent daughter relation
    // set in write.cpp :

    // particle 0 has particles 2,3,4 and 5 as daughters:
    auto p = mcps[0] ;

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

    auto d0 = p.daughters(0) ;
    auto d1 = p.daughters(1) ;
    auto d2 = p.daughters(2) ;
    auto d3 = p.daughters(3) ;

    if( ! ( d0 == mcps[2] ) )  throw std::runtime_error(" error: 1. daughter of particle 0 is not particle 2 ");
    if( ! ( d1 == mcps[3] ) )  throw std::runtime_error(" error: 2. daughter of particle 0 is not particle 3 ");
    if( ! ( d2 == mcps[4] ) )  throw std::runtime_error(" error: 3. daughter of particle 0 is not particle 4 ");
    if( ! ( d3 == mcps[5] ) )  throw std::runtime_error(" error: 4. daughter of particle 0 is not particle 5 ");


    // particle 3 has particles 6,7,8 and 9 as daughters:
    p = mcps[3] ;

    d0 = p.daughters(0) ;
    d1 = p.daughters(1) ;
    d2 = p.daughters(2) ;
    d3 = p.daughters(3) ;

    if( ! ( d0 == mcps[6] ) )  throw std::runtime_error(" error: 1. daughter of particle 3 is not particle 6 ");
    if( ! ( d1 == mcps[7] ) )  throw std::runtime_error(" error: 2. daughter of particle 3 is not particle 7 ");
    if( ! ( d2 == mcps[8] ) )  throw std::runtime_error(" error: 3. daughter of particle 3 is not particle 8 ");
    if( ! ( d3 == mcps[9] ) )  throw std::runtime_error(" error: 4. daughter of particle 3 is not particle 9 ");

  } else {
    throw std::runtime_error("Collection 'mcparticles' should be present");
  }


  //std::cout << "Fetching collection 'refs'" << std::endl;
  auto& refs = store.get<ExampleReferencingTypeCollection>("refs");
  if(refs.isValid()){
    auto ref = refs[0];
    for (auto j = ref.Clusters_begin(), end = ref.Clusters_end(); j!=end; ++j){
      for (auto i = j->Hits_begin(), end = j->Hits_end(); i!=end; ++i){
        //std::cout << "  Referenced object has an energy of " << i->energy() << std::endl;
      }
    }
  } else {
    throw std::runtime_error("Collection 'refs' should be present");
  }
  //std::cout << "Fetching collection 'OneRelation'" << std::endl;
  auto& rels = store.get<ExampleWithOneRelationCollection>("OneRelation");
  if(rels.isValid()) {
    //std::cout << "Referenced object has an energy of " << (*rels)[0].cluster().energy() << std::endl;
  } else {
    throw std::runtime_error("Collection 'OneRelation' should be present");
  }

//  std::cout << "Fetching collection 'WithVectorMember'" << std::endl;
  auto& vecs = store.get<ExampleWithVectorMemberCollection>("WithVectorMember");
  if(vecs.isValid()) {
    if (vecs.size() != 2) {
      throw std::runtime_error("Collection 'WithVectorMember' should have two elements'");
    }

    for (auto vec : vecs) {
      if (vec.count().size() != 2) {
        throw std::runtime_error("Element of 'WithVectorMember' collection should have two elements in its 'count' vector");
      }
    }
    if (vecs[0].count(0) != eventNum ||
        vecs[0].count(1) != eventNum + 10 ||
        vecs[1].count(0) != eventNum + 1 ||
        vecs[1].count(1) != eventNum + 11) {
        throw std::runtime_error("Element values of the 'count' vector in an element of the 'WithVectorMember' collection do not have the expected values");
    }

    for( auto item : vecs )
      for (auto c = item.count_begin(), end = item.count_end(); c!=end; ++c){
          std::cout << "  Counter value " << (*c) << std::endl;
    }
  } else {
    throw std::runtime_error("Collection 'WithVectorMember' should be present");
  }

  auto& comps = store.get<ExampleWithComponentCollection>("Component");
  if (comps.isValid()) {
    auto comp = comps[0];
    int a = comp.component().data.x + comp.component().data.z;
  }

  auto& arrays = store.get<ExampleWithArrayCollection>("arrays");
  if (arrays.isValid() && arrays.size() != 0) {
    auto array = arrays[0];
    if (array.myArray(1) != eventNum) {
      throw std::runtime_error("Array not properly set.");
    }
    if (array.arrayStruct().data.p.at(2) != 2*eventNum) {
      throw std::runtime_error("Array not properly set.");
    }
    if (array.structArray(0).x != eventNum) {
      throw std::runtime_error("Array of struct not properly set.");
    }
  } else {
    throw std::runtime_error("Collection 'arrays' should be present");
  }

  auto& nmspaces = store.get<ex42::ExampleWithARelationCollection>("WithNamespaceRelation");
  auto& copies = store.get<ex42::ExampleWithARelationCollection>("WithNamespaceRelationCopy");
  auto& cpytest = store.create<ex42::ExampleWithARelationCollection>("TestConstCopy");
  if (nmspaces.isValid() && copies.isValid()) {
    for (size_t j = 0; j < nmspaces.size(); j++) {
      auto nmsp = nmspaces[j];
      auto cpy = copies[j];
      cpytest.push_back(nmsp.clone());
      if (nmsp.ref().isAvailable()) {
        if (nmsp.ref().data().x != cpy.ref().data().x || nmsp.ref().data().y != cpy.ref().data().y) {
          throw std::runtime_error("Copied item has differing data in OneToOne referenced item.");
        }
        // check direct accessors of POD sub members
        if (nmsp.ref().x() != cpy.ref().x()) {
          throw std::runtime_error("Getting wrong values when using direct accessors for sub members.");
        }
        if (nmsp.number() != cpy.number()) {
          throw std::runtime_error("Copied item has differing member.");
        }
        if (!(nmsp.ref().getObjectID() == cpy.ref().getObjectID())) {
          throw std::runtime_error("Copied item has wrong OneToOne references.");
        }
      }
      auto cpy_it = cpy.refs_begin();
      for (auto it = nmsp.refs_begin(); it != nmsp.refs_end(); ++it, ++cpy_it) {
        if (it->data().x != cpy_it->data().x || it->data().y != cpy_it->data().y) {
          throw std::runtime_error("Copied item has differing data in OneToMany referenced item.");
        }
        if (!(it->getObjectID() == cpy_it->getObjectID())) {
          throw std::runtime_error("Copied item has wrong OneToMany references.");
        }
      }
    }
  } else {
    throw std::runtime_error("Collection 'WithNamespaceRelation' and 'WithNamespaceRelationCopy' should be present");
  }
}

void run_read_test(podio::IReader& reader) {
  auto store = podio::EventStore();
  store.setReader(&reader);

  const auto nEvents = reader.getEntries();

  // Some information in the test files dpends directly on the index. In
  // read-multiple, the same file is essentially read twice, so that at the file
  // change the index which is used for testing has to start at 0 again. This
  // function just wraps that. The magic number of 2000 is the number of events
  // that are present in each file that is written in write
  const auto correctIndex = [nEvents](unsigned index) {
    if (nEvents > 2000) {
      return index % (nEvents / 2);
    }
    return index;
  };

  for(unsigned i=0; i<nEvents; ++i) {

    if(i%1000==0)
      std::cout<<"reading event "<<i<<std::endl;

    processEvent(store, true, correctIndex(i));
    store.clear();
    reader.endOfEvent();
  }
}

#endif // PODIO_TESTS_READ_TEST_H__
