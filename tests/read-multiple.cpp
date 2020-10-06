#include "read_test.h"
#include "podio/ROOTReader.h"

int main(){
  auto reader = podio::ROOTReader();
  auto store = podio::EventStore();
  reader.openFiles({"example.root", "example1.root"});
  store.setReader(&reader);

  bool verbose = true;

  unsigned nEvents = reader.getEntries();
  for(unsigned i=0; i<nEvents; ++i) {
    if(i%1000==0)
      std::cout<<"reading event "<<i<<std::endl;
    processEvent(store, true, i%(nEvents / 2));
    store.clear();
    reader.endOfEvent();
  }
  reader.closeFiles();
  return 0;
}
