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
  podio::ROOTWriter writer("associations.root", &store);

  std::cout << "start writting collections...\n";
  auto& info       = store.create<EventInfoCollection>("info");
  auto& hits       = store.create<ExampleHitCollection>("hits");
  auto& clusters   = store.create<ExampleClusterCollection>("clusters");
  auto& hits_subset   = store.create<ExampleHitCollection>("hits_subset");
  hits_subset.setSubsetCollection(true);

  writer.registerForWrite("clusters");
  // writer.registerForWrite("hits");
  writer.registerForWrite("hits_subset");

  unsigned nevents = 2;

  for(unsigned i=0; i<nevents; ++i) {

    auto item1 = MutableEventInfo();
    item1.Number(i);
    info.push_back(item1);

    auto& evtMD = store.getEventMetaData() ;
    evtMD.setValue( "UserEventWeight" , (float) 100.*i );
    std::cout << " event number: " << i << std::endl;
    evtMD.setValue( "UserEventName" , std::to_string(i));

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
    cluster.addClusters( clu0 );
    cluster.addClusters( clu1 );

    // Add tracked hits to subset hits collection
    hits_subset.push_back(hit1);
    hits_subset.push_back(hit2);

    // Push all clusters
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
  reader.openFile("associations.root");

  auto store = podio::EventStore();
  store.setReader(&reader);

  const auto nEvents = reader.getEntries();

  for(unsigned i=0; i<nEvents; ++i) {

    auto& clusters = store.get<ExampleClusterCollection>("clusters");
    if(clusters.isValid()) {
      for (const auto& cluster : clusters) {
        if (cluster.isAvailable()) {
          for (const auto& hit : cluster.Hits()) {
            if (hit.isAvailable()) {
              throw std::runtime_error("Hit is available, although it has not been written");
            }
          }
        }
      }
    } else {
      throw std::runtime_error("Collection 'clusters' should be present");
    }

    // Test for subset collections
    auto& hits_subset = store.get<ExampleHitCollection>("hits_subset");
    if(hits_subset.isValid()) {
      if (!hits_subset.isSubsetCollection()) {
        throw std::runtime_error("hits_subset should be a subset collection");
      }

      if (hits_subset.size() != 2) {
        throw std::runtime_error("subset collection should have original size");
      }

      for (const auto& hit : hits_subset) {
        if (hit.isAvailable()) {
          throw std::runtime_error("Hit is available, although it has not been written");
        }
      }
    } else {
      throw std::runtime_error("Collection 'hits_subset' should be present");
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
