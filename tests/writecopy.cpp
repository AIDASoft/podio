// Data model
#include "EventInfoCollection.h"
#include "ExampleWithArrayCollection.h"
#include "StructWithArray.h"

// STL
#include <iostream>
#include <vector>

// podio specific includes
#include "podio/EventStore.h"
#include "podio/ROOTWriter.h"

int main(){

  std::cout<<"start processing"<<std::endl;

  auto store = podio::EventStore();
  auto writer = podio::ROOTWriter("example.root", &store);

  auto& info       = store.create<EventInfoCollection>("info");
  auto& arrays     = store.create<ExampleWithArrayCollection>("arrays");
  writer.registerForWrite("info");
  writer.registerForWrite("arrays");

  unsigned nevents = 2000;

  for(unsigned i=0; i<nevents; ++i) {
    if(i % 1000 == 0) {
      std::cout << "processing event " << i << std::endl;
    }

    auto item1 = EventInfo();
    item1.Number(i);
    info.push_back(item1);
    
    std::array<int, 4> arrayTest = {0, 0, 2, 3};
    std::array<int, 4> arrayTest2 = {4, 4, 2 * static_cast<int>(i)};
    NotSoSimpleStruct a;
    a.data.p = arrayTest2;
    ex2::NamespaceStruct nstruct;
    nstruct.x = static_cast<int>(i);
    std::array<ex2::NamespaceStruct, 4> structArrayTest = {nstruct, nstruct, nstruct, nstruct};
    auto array = ExampleWithArray(a, arrayTest, arrayTest, arrayTest, arrayTest, structArrayTest);
    array.myArray(1, i);
    array.arrayStruct(a);
    arrays.push_back(array);

    writer.writeEvent();
    store.clearCollections();
  }

  writer.finish();
}
