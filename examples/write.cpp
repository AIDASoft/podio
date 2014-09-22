#include "DummyData.h"
#include "DummyDataCollection.h"
#include "ReferencingDataCollection.h"

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

  // populate the first collection
  DummyDataCollection& coll = store.create<DummyDataCollection>("DummyData");
  DummyDataHandle d1 = coll.create();
  d1.setNumber(42);
  DummyDataHandle d2 = coll.create();
  d2.setNumber(23);

  // populate the second collection
  ReferencingDataCollection& coll2 = store.create<ReferencingDataCollection>("ReferencingData");
  ReferencingDataHandle rd1 = coll2.create();
  ReferencingDataHandle rd2 = coll2.create();

  // Let's set a few references between the two collections
  rd1.setDummyData(d2);
  rd2.setDummyData(d1);

  // print out for checking
  std::cout << "Printing ref collection:" << std::endl;
  for(const auto& ref : coll2){
    if (ref.DummyData().isAvailable()) {
      std::cout << "  The Referenced object has the number " << ref.DummyData().Number() << std::endl;
    } else {
      std::cout << "  Referenced object not present!" << std::endl;
    }
  }

  // and now for the writing
  // TODO: do that at a different time w/o coll pointer
  // COLIN: the tree branching is done in these functions. I was expecting it to be done before filling the tree. Couldn't the EventStore deal with this instead of the user? But that probably introduces a dependency we could do without.
  writer.registerForWrite("DummyData", coll);
  writer.registerForWrite("ReferencingData", coll2);
  writer.writeEvent();
  writer.finish();
  std::cout << "Wrote example.root with two collections (DummyData/Referencing Data)" << std::endl;

  return 0;
}
