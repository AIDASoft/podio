#include "read_test.h"
#include "podio/ROOTReader.h"

int main(){
  auto reader = podio::ROOTReader();
  auto store = podio::EventStore();
  reader.openFile("example.root");
  store.setReader(&reader);

  unsigned nEvents = reader.getEntries();
  for(unsigned i=0; i<nEvents; ++i) {
    if(i%1000==0)
      std::cout<<"reading event "<<i<<std::endl;
    processEvent(store, true, i);
    store.clear();
    reader.endOfEvent();
  }
  reader.closeFile();
  return 0;
}
