#include "read_test.h"
// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"
#include "podio/ROOTWriter.h"

#ifdef __has_include
#  if __has_include(<version>)
#    include <version>
#  endif
#  if __has_include(<ranges>)
#    include <ranges>
#  endif
#endif

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

    #if __cpp_lib_ranges
    auto& mcparticles = store.get<ExampleMCCollection>("mcparticles");

    auto v = mcparticles | std::views::reverse;

    auto is_electron = [](const auto& p){ return p.PDG() == 11; };
    for (const auto& e: mcparticles | std::views::filter(is_electron)) {
      // stuff
    }
    #endif

    store.clear();
    reader.endOfEvent();
  }
  reader.closeFile();
  return 0;
}
