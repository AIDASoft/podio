#include "DummyGenerator.h"

#include "albers/EventStore.h"
#include "albers/Writer.h"

#include <iostream>
#include <cmath>
#include <cassert>

#include "TLorentzVector.h"

#include "ParticleCollection.h"
#include "JetCollection.h"
#include "JetParticleAssociationCollection.h"


DummyGenerator::DummyGenerator(unsigned njets, int npart,
			       albers::EventStore& store) :
  m_njets(njets),
  m_pstar(0., 0.25),
  m_phi(-M_PI, M_PI),
  m_theta(0, M_PI),
  m_npart(npart),
  m_uniform(0.,1.),
  m_ptypeprob{0., 0.65, 0.85, 1.},
  m_store(store)
{
  auto& coll1 = m_store.create<ParticleCollection>("GenParticle");
  auto& coll2 = m_store.create<JetCollection>("GenJet");
  auto& coll3 = m_store.create<JetParticleAssociationCollection>("GenJetParticle");
}


void DummyGenerator::generate() {
  for( unsigned i=0; i<m_njets; ++i) {
    generate_jet();
  }
}


void DummyGenerator::generate_jet() {
  unsigned npart = m_npart(m_engine);
  std::cout<<"generate jet with nparticles = "
	   << npart << std::endl;
  // define boost vector
  JetCollection* jcoll = nullptr;
  m_store.get("GenJet", jcoll);
  JetHandle& jet = jcoll->create();
  JetParticleAssociationCollection* acoll = nullptr;
  m_store.get("GenJetParticle", acoll);
  for(unsigned i=0; i<npart; ++i) {
    ParticleHandle ptc = generate_particle(); // pass boost
    JetParticleAssociationHandle& assoc = acoll->create();
    assoc.setJet(jet);
    assoc.setParticle(ptc);
  }
}

ParticleHandle DummyGenerator::generate_particle() {
  float phistar = m_phi(m_engine);
  float thetastar = m_theta(m_engine);
  float etastar = -log ( tan(thetastar/2.) );
  float pstar = m_pstar(m_engine);
  float ftype = m_uniform(m_engine);
  unsigned itype = -1;
  for(unsigned i=0; i<m_ptypeprob.size(); ++i) {
    if(ftype<m_ptypeprob[i]) {
      itype = i;
      break;
    }
  }
  assert(itype < m_ptypeprob.size());
  TLorentzVector p4star;
  std::cout<<"\tparticle "<<itype<<" "<<pstar<<" "<<thetastar<<" "<<etastar<<" "<<phistar<<std::endl;

  float phi = phistar;
  float eta = etastar;
  float pt = pstar;
  int id = itype;

  float mass = 0;
  switch(itype) {
  case 1:
    mass = 0.135; break;
  case 3:
    mass = 0.497; break;
  default: break;
  }

  ParticleCollection* pcoll = nullptr;
  m_store.get("GenParticle", pcoll);
  ParticleHandle& ptc = pcoll->create();
  LorentzVector lv;
  lv.Phi  = phi;
  lv.Eta  = eta;
  lv.Mass = mass;
  lv.Pt   = pt;
  ptc.setP4(lv);
  ptc.setID(id);

  return ptc;
  // boost particle
  // return particle POD
}
