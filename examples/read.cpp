#include "ParticleCollection.h"
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

int main(){
  gSystem->Load("libDataModelExample.so");
  albers::Reader reader;
  albers::EventStore store(nullptr);
  store.setReader(&reader);
  reader.openFile("example.root");

  ParticleCollection* refs(nullptr);
  bool present = store.get("Particle",refs);

  if (present){
    std::cout << "Printing Particle collection:" << std::endl;
    for(const auto& ref : *refs){
      std::cout << "particle: " << ref.ID()<< " " << ref.P4().Mass() << std::endl;
    }
  }

  return 0;
}
