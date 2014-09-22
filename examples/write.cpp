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

int main(){
  gSystem->Load("libDataModelExample.so");

  albers::Registry   registry;
  albers::EventStore store(&registry);
  albers::Writer     writer("example.root", &registry);

  // populate a particle collection
  // LorentzVector part
  LorentzVectorCollection& lvcoll = store.create<LorentzVectorCollection>("ParticleP4");
  LorentzVectorHandle lv1 = lvcoll.create();
  lv1.setPhi(0.);
  lv1.setEta(1.);
  lv1.setMass(125.);
  lv1.setPt(50.);

  // particle part
  ParticleCollection& partcoll = store.create<ParticleCollection>("Particle");
  ParticleHandle p1 = partcoll.create();
  p1.setID(25);
  p1.setP4(lv1);

  // and now for the writing
  // TODO: do that at a different time w/o coll pointer
  // COLIN: the tree branching is done in these functions. I was expecting it to be done before filling the tree. Couldn't the EventStore deal with this instead of the user? But that probably introduces a dependency we could do without.
  writer.registerForWrite("ParticleP4", lvcoll);
  writer.registerForWrite("Particle", partcoll);
  writer.writeEvent();
  writer.finish();
  std::cout << "Wrote example.root with two collections" << std::endl;

  return 0;
}
