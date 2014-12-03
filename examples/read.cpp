#include "ParticleCollection.h"
#include "EventInfoCollection.h"
#include "JetCollection.h"
#include "JetParticleAssociationCollection.h"

// Utility functions
#include "JetUtils.h"
#include "VectorUtils.h"
#include "ParticleUtils.h"

// ROOT
#include "TBranch.h"
#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TLorentzVector.h"

// STL
#include <vector>
#include <iostream>

// albers specific includes
#include "albers/EventStore.h"
#include "albers/Reader.h"
#include "albers/Registry.h"

void processEvent(albers::EventStore& store, bool verbose,
		  albers::Reader& reader) {

  // read event information
  EventInfoCollection* evinfocoll(nullptr);
  bool evinfo_available = store.get("EventInfo",evinfocoll);
  if(evinfo_available) {
    const EventInfoHandle& evinfoHandle = evinfocoll->get(0);
    const EventInfo& evinfo = evinfoHandle.read();
    if(verbose)
      std::cout << "event number " << evinfo.Number << std::endl;
    // COLIN avoid bug at first event
    if(evinfo.Number==0) {
      std::cerr<<"skipping bugged first event"<<std::endl;
      return;
    }
  }

  // the following is commented out to test on-demand reading through Jet-Particle association,
  // see below
  // // read particles
  // ParticleCollection* ptcs(nullptr);
  // bool particles_available = store.get("GenParticle",ptcs);
  // if (particles_available){
  //   for(const auto& part : *ptcs) {
  //     std::cout<<part.containerID()<<" "<<part.index()<<std::endl;
  //   }
  // }

  // read jets
  JetCollection* jrefs(nullptr);
  bool jets_available = store.get("GenJet",jrefs);
  std::vector<ParticleHandle> injets;

  if (jets_available){
    JetParticleAssociationCollection* jprefs(nullptr);
    bool assoc_available = store.get("GenJetParticle",jprefs);
    if(verbose) {
      reader.getRegistry()->print();
      std::cout << "jet collection:" << std::endl;
    }
    for(const auto& jet : *jrefs){
      std::vector<ParticleHandle> jparticles = utils::associatedParticles(jet,
									  *jprefs);
      TLorentzVector lv = utils::lvFromPOD(jet.read().Core.P4);
      if(verbose)
	std::cout << "\tjet: E=" << lv.E() << " "<<lv.Eta()<<" "<<lv.Phi()
		  <<" npart="<<jparticles.size()<<std::endl;
      if(assoc_available) {
	for(const auto& part : jparticles) {
	  if(part.isAvailable()) {
	    if(verbose)
	      std::cout<<"\t\tassociated "<<part<<std::endl;
	    injets.push_back(part);
	  }
	}
      }
    }
  }

  // read particles
  ParticleCollection* ptcs(nullptr);
  bool particles_available = store.get("GenParticle",ptcs);
  if (particles_available){
    std::vector<ParticleHandle> muons;
    if(verbose)
      std::cout << "particle collection:" << std::endl;
    for(const auto& ptc : *ptcs){
      if(verbose)
	std::cout<<"\t"<<ptc<<std::endl;
      if( ptc.read().Core.Type == 4 )
	muons.push_back(ptc);
    }
    // listing particles that are not used in a jet
    const std::vector<ParticleHandle>& particles = ptcs->getHandles();
    std::vector<ParticleHandle> unused = utils::unused(particles, injets);
    if(verbose)
      std::cout<<"unused particles: "<<unused.size()<<"/"<<particles.size()<<" "<<injets.size()<<std::endl;

    // computing isolation for first muon
    if(not muons.empty()) {
      const ParticleHandle& muon = muons[0];
      float dRMax = 0.5;
      const std::vector<ParticleHandle> incone = utils::inCone( muon.read().Core.P4,
								particles,
								dRMax);
      float sumpt = utils::sumPt(incone);
      if( verbose ) {
	std::cout<<"muon: "<<muon<<" sumpt "<<sumpt<<std::endl;
	std::cout<<"\tparticles in cone:"<<std::endl;
      }
      for(const auto& ptc : incone) {
	if( verbose )
	  std::cout<<"\t"<<ptc<<std::endl;
      }
    }
  }
}


int main(){
  gSystem->Load("libDataModelExample.so");
  albers::Reader reader;
  albers::EventStore store(nullptr);
  store.setReader(&reader);
  reader.openFile("example.root");

  bool verbose = true;

  // unsigned nEvents = 5;
  unsigned nEvents = reader.getEntries();
  for(unsigned i=0; i<nEvents; ++i) {
    if(i%1000==0)
      std::cout<<"reading event "<<i<<std::endl;
    if(i>10)
      verbose = false;
    processEvent(store, verbose, reader);
    store.endOfEvent();
    reader.endOfEvent();
  }
  return 0;
}
