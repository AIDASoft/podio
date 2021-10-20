#include "read_test.h"

#include "podio/EventStore.h"
#include "podio/SIOWriter.h"
#include "podio/SIOReader.h"

int main() {
  podio::SIOReader reader;
  reader.openFile("example.sio");

  auto store = podio::EventStore();
  store.setReader(&reader);

  auto writer = podio::SIOWriter("example_copy.sio", &store);
  writer.registerForWrite("info");
  writer.registerForWrite("mcparticles");
  writer.registerForWrite("hits");
  writer.registerForWrite("clusters");

  unsigned nEvents = reader.getEntries();
  for(unsigned i=0; i<nEvents; ++i) {
    if(i%1000==0)
      std::cout<<"reading event "<<i<<std::endl;
    processEvent(store, i, reader.currentFileVersion());

    writer.writeEvent();

    store.clear();
    reader.endOfEvent();
  }
  reader.closeFile();
  return 0;
}
