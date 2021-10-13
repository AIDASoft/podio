#include "podio/ROOTWriter.h"
#include "podio/ROOTReader.h"
#include "podio/EventStore.h"

#include "datamodel/EventInfoCollection.h"
#include "datamodel/ExampleHitCollection.h"
#include "datamodel/ExampleClusterCollection.h"

#include <string>
#include <iostream>

void writeCollection() {

  auto store = podio::EventStore();
  podio::ROOTWriter writer("example1.root", &store);

  std::cout << "start writting collections...\n";
  auto& info       = store.create<EventInfoCollection>("info");
  auto& hits       = store.create<ExampleHitCollection>("hits");
  auto& clusters   = store.create<ExampleClusterCollection>("clusters");

  writer.registerForWrite("clusters");
  // writer.registerForWrite("hits");

  unsigned nevents = 2;

  for(unsigned i=0; i<nevents; ++i) {

    auto item1 = MutableEventInfo();
    item1.Number(i);
    info.push_back(item1);

    auto& evtMD = store.getEventMetaData() ;
    evtMD.setValue( "UserEventWeight" , (float) 100.*i ) ;
    std::cout << " event number: " << i << std::endl;
    evtMD.setValue( "UserEventName" , std::to_string(i)) ;

    auto hit1 = ExampleHit( 0xbad, 0.,0.,0.,23.+i);
    auto hit2 = ExampleHit( 0xcaffee,1.,0.,0.,12.+i);

    hits.push_back(hit1);
    hits.push_back(hit2);

    // Clusters
    auto cluster  = MutableExampleCluster();
    auto clu0  = MutableExampleCluster();
    auto clu1  = MutableExampleCluster();

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

    writer.writeEvent();
    store.clearCollections();    
  }
  writer.finish();
}

void readCollection() {

  // Start reading the input
  auto reader = podio::ROOTReader();
  reader.openFile("example1.root");

  auto store = podio::EventStore();
  store.setReader(&reader);

  const auto nEvents = reader.getEntries();

  for(unsigned i=0; i<nEvents; ++i) {

    auto& clusters = store.get<ExampleClusterCollection>("clusters");
    if(clusters.isValid()){
      // // auto cluster = clusters[0];
      // // for (auto j = cluster.Hits_begin(), end = cluster.Hits_end(); j!=end; ++i){
        // // std::cout << "  Referenced hit has an energy of " << j->energy() << std::endl;
      // // }
    } else {
      throw std::runtime_error("Collection 'clusters' should be present");
    }
  
    store.clear();
    reader.endOfEvent();
  }

}

int main() {

  writeCollection();
  readCollection();

  return 0;
}
