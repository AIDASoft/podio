#ifndef TEST_DUMMYGENERATOR_H
#define TEST_DUMMYGENERATOR_H

#include <random>

namespace albers {
  class EventStore;
  class Writer;
}

class ParticleHandle;

class DummyGenerator {
  /// COLIN need verbose flag.

public:
  /// COLIN not good to pass the writer to DummyGenerator. need to change writer interface not to need collection reference, and then this parameter can be removed
  DummyGenerator(unsigned njets, int npart,
		 albers::EventStore& store,
		 albers::Writer& writer);
  void generate();

private:
  /// generate a jet
  void generate_jet();

  /// generate a particle
  ParticleHandle generate_particle();

  /// number of jets
  unsigned m_njets;

  /// random engine
  std::default_random_engine m_engine;

  /// energy in jet center of mass
  std::lognormal_distribution<float> m_pstar;

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
};

#endif
