#include "DummyDataCollection.h"
#include "ReferencingDataCollection.h"
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
  ReferencingDataCollection* refs(nullptr);
  bool refsPresent = store.get("ReferencingData",refs);

  if (refsPresent){
    std::cout << "Printing ref collection:" << std::endl;
    for(const auto& ref : *refs){
      std::cout << "  The Referenced object has the number "
	        << ref.DummyData().Number() << std::endl;
    }
  }

  return 0;
}
