// STL
#include <vector>
#include <iostream>

// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

// test data model
#include "ExampleHitCollection.h"
#include "ExampleClusterCollection.h"
#include "ExampleReferencingTypeCollection.h"
#include "ExampleWithOneRelationCollection.h"
#include "ExampleWithVectorMemberCollection.h"

int glob = 0;

void processEvent(podio::EventStore& store, bool verbose,
      podio::ROOTReader& reader) {

  const ExampleClusterCollection* clusters = nullptr;
  //std::cout << "Fetching collection 'clusters'" << std::endl;
  bool is_available = store.get("clusters",clusters);
  if(is_available){
    auto cluster = (*clusters)[0];
    //std::cout << "Cluster has an energy of " << cluster.energy() << std::endl;
    for (auto i = cluster.Hits_begin(), end = cluster.Hits_end(); i!=end; ++i){
      //std::cout << "  Referenced hit has an energy of " << i->energy() << std::endl;
      glob++;
    }
  }
  const ExampleReferencingTypeCollection* refs = nullptr;
  //std::cout << "Fetching collection 'refs'" << std::endl;
  is_available = store.get("refs",refs);
  if(is_available){
    auto ref = (*refs)[0];
    for (auto j = ref.Clusters_begin(), end = ref.Clusters_end(); j!=end; ++j){
      for (auto i = j->Hits_begin(), end = j->Hits_end(); i!=end; ++i){
        //std::cout << "  Referenced object has an energy of " << i->energy() << std::endl;
        glob++;
      }
    }
  }
  const ExampleWithOneRelationCollection* rels = nullptr;
  //std::cout << "Fetching collection 'OneRelation'" << std::endl;
  is_available = store.get("OneRelation",rels);
  if(is_available) {
    //std::cout << "Referenced object has an energy of " << (*rels)[0].cluster().energy() << std::endl;
    glob++;
  }
  const ExampleWithVectorMemberCollection* vecs = nullptr;
//  std::cout << "Fetching collection 'WithVectorMember'" << std::endl;
  is_available = store.get("WithVectorMember",vecs);
  if(is_available) {
    auto item = (*vecs)[0];
//    std::cout << (*vecs).size() << std::endl;
    for (auto c = item.count().begin(), end = item.count().end(); c!=end; ++c){
      //std::cout << "  Counter value " << (*c) << std::endl;
      glob++;
    }
  }
}

int main(){
  auto reader = podio::ROOTReader();
  auto store = podio::EventStore();
  reader.openFile("example.root");
  store.setReader(&reader);

  bool verbose = true;

  unsigned nEvents = reader.getEntries();
  for(unsigned i=0; i<nEvents; ++i) {
    if(i%1000==0)
      std::cout<<"reading event "<<i<<std::endl;
    processEvent(store, true, reader);
    store.clear();
    reader.endOfEvent();
  }
  return 0;
}
