#include "EventInfo.h"
#include "EventInfoCollection.h"
#include "Particle.h"
#include "ParticleCollection.h"
#include "LorentzVector.h"
#include "LorentzVectorCollection.h"

#include "TBranch.h"
#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"

#include <iostream>
#include <vector>

// albers specific includes
#include "albers/EventStore.h"
#include "albers/Registry.h"
#include "albers/Writer.h"



void processEvent(unsigned iEvent, albers::EventStore& store, albers::Writer& writer) {
  if(iEvent % 1000 == 0)
    std::cout<<"processing event "<<iEvent<<std::endl;

  // fill event information
  EventInfoCollection* evinfocoll = nullptr;
  store.get("EventInfo", evinfocoll);
  if(evinfocoll==nullptr) {
    std::cerr<<"collection EventInfo does not exist!"<<std::endl;
    return;
  }
  EventInfoHandle& evinfo = evinfocoll->create();
  evinfo.setNumber(iEvent);

  // populate a particle collection
  // LorentzVector part
  LorentzVectorCollection* lvcoll = nullptr;
  store.get("ParticleP4", lvcoll);
  if(lvcoll==nullptr) {
    //COLIN: set up exception in store::get
    std::cerr<<"collection ParticleP4 does not exist!"<<std::endl;
    return;
  }
  LorentzVectorHandle& lv1 = lvcoll->create();
  lv1.setPhi(0.);
  lv1.setEta(1.);
  lv1.setMass(iEvent);
  lv1.setPt(50.);
  // particle part
  ParticleCollection* partcoll = nullptr;
  store.get("Particle", partcoll);
  if(lvcoll==nullptr) {
    std::cerr<<"collection Particle does not exist!"<<std::endl;
    return;
  }
  ParticleHandle& p1 = partcoll->create();
  p1.setID(25 + iEvent);
  p1.setP4(lv1);

  // and now for the writing
  // TODO: do that at a different time w/o coll pointer
  // COLIN: calling writeEvent should not be left up to the user.
  writer.writeEvent();
  store.next();

  return;
}


int main(){
  gSystem->Load("libDataModelExample.so");

  std::cout<<"start processing"<<std::endl;

  albers::Registry   registry;
  albers::EventStore store(&registry);
  albers::Writer     writer("example.root", &registry);

  unsigned nevents=10;

  EventInfoCollection& evinfocoll = store.create<EventInfoCollection>("EventInfo");

  // particle Lorentz vectors are stored in a separate collection
  LorentzVectorCollection& lvcoll = store.create<LorentzVectorCollection>("ParticleP4");

  // particle part
  ParticleCollection& partcoll = store.create<ParticleCollection>("Particle");

  writer.registerForWrite("EventInfo", evinfocoll);
  writer.registerForWrite("ParticleP4", lvcoll);
  writer.registerForWrite("Particle", partcoll);

  for(unsigned i=0; i<nevents; ++i) {
    processEvent(i, store, writer);
  }

  writer.finish();
}
