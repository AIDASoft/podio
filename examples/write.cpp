#include "EventInfo.h"
#include "EventInfoCollection.h"
#include "Particle.h"
#include "ParticleCollection.h"
#include "LorentzVector.h"
#include "Jet.h"
#include "JetCollection.h"

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

  LorentzVector lv1;
  lv1.Phi  = 0;
  lv1.Eta  = 1 ;
  lv1.Mass = 125;
  lv1.Pt   = 50.;

  // particle part
  ParticleCollection* partcoll = nullptr;
  store.get("Particle", partcoll);
  ParticleHandle& p1 = partcoll->create();
  p1.setID(25 + iEvent);
  p1.setP4(lv1);
  ParticleHandle& p2 = partcoll->create();
  p2.setID(42 + iEvent);
  p2.setP4(lv1);

  // jet part
  JetCollection* jetcoll = nullptr;
  store.get("Jet", jetcoll);
  JetHandle& j1 = jetcoll->create();
  j1.setP4(lv1);
  j1.addparticles(p1);
  j1.addparticles(p2);   
  for(auto i = j1.particles_begin(), e = j1.particles_end(); i!=e;++i){
    std::cout << "  component pt: " << i->P4().Pt << std::endl;
  }

  // and now for the writing
  // TODO: do that at a different time w/o coll pointer
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

  unsigned nevents=10000;

  EventInfoCollection& evinfocoll = store.create<EventInfoCollection>("EventInfo");

  // particle part
  ParticleCollection& partcoll = store.create<ParticleCollection>("Particle");
  JetCollection& jetcoll = store.create<JetCollection>("Jet");

  writer.registerForWrite("EventInfo", evinfocoll);
  writer.registerForWrite("Particle", partcoll);
  writer.registerForWrite("Jet", jetcoll);

  for(unsigned i=0; i<nevents; ++i) {
    processEvent(i, store, writer);
  }

  writer.finish();
}
