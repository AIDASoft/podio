#include "DummyGenerator.h"

#include "albers/EventStore.h"
#include "albers/Writer.h"

#include <iostream>
#include <cmath>
#include <cassert>

#include "TLorentzVector.h"
#include "TVector3.h"

#include "ParticleCollection.h"
#include "JetCollection.h"
#include "JetParticleAssociationCollection.h"

#include "VectorUtils.h"

DummyGenerator::DummyGenerator(int npart,
			       albers::EventStore& store) :
  m_njets(2), // not used
  m_engine(0xdeadbeef),
  m_pstar(0., 0.25),
  m_phi(-M_PI, M_PI),
  m_theta(0, M_PI),
  m_npart(npart),
  m_uniform(0.,1.),
  m_ptypeprob{0., 0.65, 0.85, 1.},
  m_store(store),
  m_nprint(-1),
  m_ievt(0) {

  auto& coll1 = m_store.create<ParticleCollection>("GenParticle");
  auto& coll2 = m_store.create<JetCollection>("GenJet");
  auto& coll3 = m_store.create<JetParticleAssociationCollection>("GenJetParticle");
}


void DummyGenerator::generate() {
  generate_jet(200., TVector3(1,0,0) );
  generate_jet(200., TVector3(-1,0,0) );
  m_ievt++;
}


void DummyGenerator::generate_jet(float energy, const TVector3& direction) {
  unsigned npart = m_npart(m_engine);
  if(npart<2) return; // cannot have energy momentum conservation with a single particle
  if (m_ievt<m_nprint)
    std::cout<<"generate jet with nparticles = "
	     << npart << std::endl;

  JetCollection* jcoll = nullptr;
  m_store.get("GenJet", jcoll);
  JetHandle& jet = jcoll->create();
  JetParticleAssociationCollection* acoll = nullptr;
  m_store.get("GenJetParticle", acoll);
  TLorentzVector p4star;

  // keeping track of all created particles to boost them later on
  std::vector<ParticleHandle> particles;

  for(unsigned i=0; i<npart-1; ++i) {
    bool success = false;
    while(not success) {
      auto result = generate_particle();
      if(not result.first)
	continue;
      else {
	ParticleHandle& ptc = *(result.second);
	p4star += utils::lvFromPOD( ptc.P4() );
	JetParticleAssociationHandle& assoc = acoll->create();
	assoc.setJet(jet);
	assoc.setParticle(ptc);
	particles.push_back(ptc);
	success = true;
      }
    }
  }

  // last particle is created to allow vector momentum conservation in jet com frame
  auto result = generate_particle(&p4star);
  ParticleHandle& ptc = *(result.second);
  TLorentzVector final = utils::lvFromPOD( ptc.P4() );
  p4star += final;
  JetParticleAssociationHandle& assoc = acoll->create();
  assoc.setJet(jet);
  assoc.setParticle(ptc);
  particles.push_back(ptc);

  // now boosting all particles to lab frame
  TLorentzVector jetlv;
  float gamma = energy/p4star.M();
  float beta = sqrt(1 - 1/(gamma*gamma));
  TVector3 boost(direction);
  boost *= static_cast<double>(beta);
  for(ParticleHandle& ptc : particles) {
    TLorentzVector lv = utils::lvFromPOD( ptc.P4() );
    lv.Boost( boost );
    ptc.setP4( utils::lvToPOD(lv) );
    jetlv += lv;
  }
  jet.setP4( utils::lvToPOD(jetlv) );
}

std::pair<bool, ParticleHandle*> DummyGenerator::generate_particle(const TLorentzVector* lv) {

  // particle type and mass
  float ftype = m_uniform(m_engine);
  unsigned itype = -1;
  for(unsigned i=0; i<m_ptypeprob.size(); ++i) {
    if(ftype<m_ptypeprob[i]) {
      itype = i;
      break;
    }
  }
  assert(itype < m_ptypeprob.size());
  float mass = 0.;
  switch(itype) {
  case 1:
    mass = 0.135; break;
  case 3:
    mass = 0.497; break;
  default: break;
  }

  LorentzVector lvpod;
  if( lv == nullptr ) {
    float phistar = m_phi(m_engine);
    float thetastar = m_theta(m_engine);
    float etastar = -log ( tan(thetastar/2.) );
    if(fabs(etastar)>5.)
      return std::make_pair<bool, ParticleHandle*>(false, nullptr);
    float ptstar = -1;
    while(ptstar<0 || ptstar>0.5) {
      ptstar = m_pstar(m_engine);
    }

    float phi = phistar;
    float eta = etastar;
    float pt = ptstar;

    lvpod.Phi  = phi;
    lvpod.Eta  = eta;
    lvpod.Mass = mass;
    lvpod.Pt   = pt;
  }
  else{
    float pmag = lv->Vect().Mag();
    float energy = sqrt(pmag*pmag + mass*mass);
    TLorentzVector opposite( -lv->Px(), -lv->Py(), -lv->Pz(), energy);
    lvpod = utils::lvToPOD(opposite);
  }
  int id = itype;

  ParticleCollection* pcoll = nullptr;
  m_store.get("GenParticle", pcoll);
  ParticleHandle& ptc = pcoll->create();
  ptc.setID(id);
  ptc.setP4( lvpod );

  if(m_ievt<m_nprint) {
    TLorentzVector lv = utils::lvFromPOD(ptc.P4());
    std::cout<<"\tparticle "<<ptc.ID()<<" "<<lv.Eta()<<" "<<lv.Phi()<<" "<<lv.Pt()<<" "<<lv.E()<<std::endl;
  }

  return std::make_pair<bool, ParticleHandle*>(true, &ptc);
}
