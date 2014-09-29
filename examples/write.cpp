// Data model
#include "EventInfo.h"
#include "EventInfoCollection.h"
#include "Particle.h"
#include "ParticleCollection.h"
#include "JetCollection.h"
#include "JetParticleAssociationCollection.h"
#include "LorentzVector.h"

// Utility functions
#include "VectorUtils.h"

#include "TLorentzVector.h"

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


ParticleHandle& createParticle(int ID, float eta, float phi, float mass, float pt, ParticleCollection* coll) {
  LorentzVector lv1;
  lv1.Phi  = phi;
  lv1.Eta  = eta;
  lv1.Mass = mass;
  lv1.Pt   = pt;

  ParticleHandle& p1 = coll->create();
  p1.setID(ID);
  p1.setP4(lv1);
  return p1;
}

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

  // particles
  ParticleCollection* partcoll = nullptr;
  store.get("Particle", partcoll);
  ParticleHandle& p1 = createParticle(22, 1, 2, 0, 50, partcoll);
  ParticleHandle& p2 = createParticle(211, 1.2, 2.2, 0.135, 40, partcoll);

  // a jet
  JetCollection* jetcoll = nullptr;
  store.get("Jet", jetcoll);
  // convert LorentzVector PODs to TLorentzVector to benefit
  // from the functionalities of this class (here, summation)
  TLorentzVector lv1 = utils::lvFromPOD(p1.P4()) + utils::lvFromPOD(p2.P4());
  JetHandle& jet = jetcoll->create();
  // convert the lorentz vector back to a POD for storage
  jet.setP4( utils::lvToPOD(lv1));

  // and the jet-particle associations
  JetParticleAssociationCollection* jetpartcoll = nullptr;
  store.get("JetParticleAssociation", jetpartcoll);
  JetParticleAssociationHandle& assoc1 = jetpartcoll->create();
  assoc1.setJet(jet);
  assoc1.setParticle(p1);
  JetParticleAssociationHandle& assoc2 = jetpartcoll->create();
  assoc2.setJet(jet);
  assoc2.setParticle(p2);


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

  // particle collection
  ParticleCollection& partcoll = store.create<ParticleCollection>("Particle");

  // jet collection
  JetCollection& jetcoll = store.create<JetCollection>("Jet");

  // jet-particle association collection
  JetParticleAssociationCollection& jetpartcoll = store.create<JetParticleAssociationCollection>("JetParticleAssociation");

  writer.registerForWrite("EventInfo", evinfocoll);
  writer.registerForWrite("Particle", partcoll);
  writer.registerForWrite("Jet", jetcoll);
  writer.registerForWrite("JetParticleAssociation", jetpartcoll);

  for(unsigned i=0; i<nevents; ++i) {
    processEvent(i, store, writer);
  }

  writer.finish();
}
