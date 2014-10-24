#ifndef TEST_DUMMYGENERATOR_H
#define TEST_DUMMYGENERATOR_H

#include <random>

namespace albers {
  class EventStore;
  class Writer;
}

class ParticleHandle;
class TLorentzVector;
class TVector3;

class DummyGenerator {
  /// COLIN need verbose flag.

public:
  /// npart : number of particles per jet
  DummyGenerator(int npart, albers::EventStore& store);

  /// generate an event. the products are stored in the event store
  void generate();

  /// set number of events to print
  void setNPrint(unsigned nprint) {m_nprint = nprint;}


private:
  /// generate a jet
  void generate_jet(float energy, const TVector3& direction);

  /// generate a particle
  std::pair<bool, ParticleHandle*> generate_particle(const TLorentzVector* lv = nullptr, int type=-1);

  /// number of jets
  unsigned m_njets;

  /// random engine
  std::default_random_engine m_engine;

  /// energy in jet center of mass
  std::normal_distribution<float> m_pstar;

  /// phi in jet center of mass
  std::uniform_real_distribution<float> m_phi;

  /// theta in jet center of mass
  std::uniform_real_distribution<float> m_theta;

  /// number of particles in jet
  std::poisson_distribution<unsigned> m_npart;

  /// various stuff
  std::uniform_real_distribution<float> m_uniform;

  /// sharing between particle types
  std::vector<float> m_ptypeprob;

  albers::EventStore& m_store;

  unsigned m_nprint;
  unsigned m_ievt;
};

#endif
