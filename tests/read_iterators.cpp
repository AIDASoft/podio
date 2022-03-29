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

    // forward loop with post-increment
    auto mc_first_electron_forward_post = mcparticles.end();
    for (auto p = mcparticles.begin(); p != mcparticles.end(); p++) {
      if (p->PDG() == 11) {
        mc_first_electron_forward_post = p;
        break;
      }
    }
    if (mc_first_electron_forward_post == mcparticles.end()) {
      // no electron found
    }

    // forward loop with pre-increment
    auto mc_first_electron_forward_pre = mcparticles.end();
    for (auto p = mcparticles.begin(); p != mcparticles.end(); ++p) {
      if (p->PDG() == 11) {
        mc_first_electron_forward_pre = p;
        break;
      }
    }
    if (mc_first_electron_forward_pre == mcparticles.end()) {
      // no electron found
    }

    // forward loop with post-increment
    auto mc_first_electron_forward_const_post = mcparticles.cend();
    for (auto p = mcparticles.cbegin(); p != mcparticles.cend(); p++) {
      if (p->PDG() == 11) {
        mc_first_electron_forward_const_post = p;
        break;
      }
    }
    if (mc_first_electron_forward_const_post == mcparticles.cend()) {
      // no electron found
    }

    // forward loop with pre-increment
    auto mc_first_electron_forward_const_pre = mcparticles.cend();
    for (auto p = mcparticles.cbegin(); p != mcparticles.cend(); ++p) {
      if (p->PDG() == 11) {
        mc_first_electron_forward_const_pre = p;
        break;
      }
    }
    if (mc_first_electron_forward_const_pre == mcparticles.cend()) {
      // no electron found
    }

    // reverse loop with post-increment
    auto mc_first_electron_reverse_post = mcparticles.rend();
    for (auto p = mcparticles.rbegin(); p != mcparticles.rend(); p++) {
      if (p->PDG() == 11) {
        mc_first_electron_reverse_post = p;
        break;
      }
    }
    if (mc_first_electron_reverse_post == mcparticles.rend()) {
      // no electron found
    }

    // reverse loop with pre-increment
    auto mc_first_electron_reverse_pre = mcparticles.rend();
    for (auto p = mcparticles.rbegin(); p != mcparticles.rend(); ++p) {
      if (p->PDG() == 11) {
        mc_first_electron_reverse_pre = p;
        break;
      }
    }
    if (mc_first_electron_reverse_pre == mcparticles.rend()) {
      // no electron found
    }

    // reverse loop with post-increment
    auto mc_first_electron_reverse_const_post = mcparticles.crend();
    for (auto p = mcparticles.crbegin(); p != mcparticles.crend(); p++) {
      if (p->PDG() == 11) {
        mc_first_electron_reverse_const_post = p;
        break;
      }
    }
    if (mc_first_electron_reverse_const_post == mcparticles.crend()) {
      // no electron found
    }

    // reverse loop with pre-increment
    auto mc_first_electron_reverse_const_pre = mcparticles.crend();
    for (auto p = mcparticles.crbegin(); p != mcparticles.crend(); ++p) {
      if (p->PDG() == 11) {
        mc_first_electron_reverse_const_pre = p;
        break;
      }
    }
    if (mc_first_electron_reverse_const_pre == mcparticles.crend()) {
      // no electron found
    }

    store.clear();
    reader.endOfEvent();
  }
  reader.closeFile();
  return 0;
}
