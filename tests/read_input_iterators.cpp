#include "read_test.h"
// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"
#include "podio/ROOTWriter.h"

int main(){
  auto reader = podio::ROOTReader();
  auto store = podio::EventStore();
  reader.openFile("example.root");
  store.setReader(&reader);

  unsigned nEvents = reader.getEntries();
  for(unsigned i=0; i<nEvents; ++i) {
    if(i%1000==0)
      std::cout<<"reading event "<<i<<std::endl;
    processEvent(store, i, reader.currentFileVersion());

    auto& mcparticles = store.get<ExampleMCCollection>("mcparticles");

    // STL find_if with lambda
    const auto mcelectrons = std::find_if(
      mcparticles.begin(), mcparticles.end(),
      [](const auto& p){ return p.PDG() == 11; });

    // ranged-based for loop
    for (const auto& p: mcparticles) {
      if (p.PDG() == 11) {
        // do something
      }
    }

    // loop with post-increment
    auto mc_first_electron = mcparticles.end();
    for (auto p = mcparticles.begin(); p != mcparticles.end(); p++) {
      if (p->PDG() == 11) {
        mc_first_electron = p;
        break;
      }
    }
    if (mc_first_electron == mcparticles.end()) {
      // no electron found
    }

    // loop with pre-increment
    mc_first_electron = mcparticles.end();
    for (auto p = mcparticles.begin(); p != mcparticles.end(); ++p) {
      if (p->PDG() == 11) {
        mc_first_electron = p;
        break;
      }
    }
    if (mc_first_electron == mcparticles.end()) {
      // no electron found
    }

    store.clear();
    reader.endOfEvent();
  }
  reader.closeFile();
  return 0;
}
