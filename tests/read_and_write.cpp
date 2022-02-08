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

  // test writing a subset of the event to a new file
  auto writer = podio::ROOTWriter("example_copy.root", &store);
  writer.registerForWrite("info");
  writer.registerForWrite("mcparticles");
  writer.registerForWrite("hits");
  writer.registerForWrite("clusters");

  unsigned nEvents = reader.getEntries();
  for(unsigned i=0; i<nEvents; ++i) {
    if(i%1000==0)
      std::cout<<"reading event "<<i<<std::endl;
    processEvent(store, i, reader.currentFileVersion());

    writer.writeEvent() ;
    
    store.clear();
    reader.endOfEvent();
  }
  reader.closeFile();
  return 0;
}
