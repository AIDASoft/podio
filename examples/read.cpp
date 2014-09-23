#include "ParticleCollection.h"
#include "EventInfoCollection.h"
#include "TBranch.h"
#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "TROOT.h"
#include <vector>
#include <iostream>

// albers specific includes
#include "albers/EventStore.h"
#include "albers/Reader.h"
#include "albers/Registry.h"


void processEvent(albers::EventStore& store) {

  // read event information
  EventInfoCollection* evinfocoll(nullptr);
  bool evinfo_available = store.get("EventInfo",evinfocoll);
  if(evinfo_available) {
    const EventInfoHandle& evinfo = evinfocoll->get(0);
    std::cout << "Reading event " << evinfo.Number() << std::endl;
  }

  // read particles
  ParticleCollection* refs(nullptr);
  bool particles_available = store.get("Particle",refs);
  if (particles_available){
    std::cout << "Printing Particle collection:" << std::endl;
    for(const auto& ref : *refs){
      std::cout << "particle: " << ref.ID() << " " << ref.P4().Mass() << std::endl;
    }
  }
}


int main(){
  gSystem->Load("libDataModelExample.so");
  albers::Reader reader;
  albers::EventStore store(nullptr);
  store.setReader(&reader);
  reader.openFile("example.root");

  // unsigned nEvents = 10;
  unsigned nEvents = reader.getEntries();
  for(unsigned i=0; i<nEvents; ++i) {
    processEvent(store);
    reader.endOfEvent();
    store.endOfEvent();
    reader.getRegistry()->resetAddresses();
  }
  return 0;
}
