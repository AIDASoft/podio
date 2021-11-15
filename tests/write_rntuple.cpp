
#include "podio/ROOTNTupleWriter.h"
#include "podio/EventStore.h"

//TODO: use write_test.h once issue with rntuple and int16_t is fixed
//#include "write_test.h"
#include "write_test_rntuple.h"

#include "podio/UserDataCollection.h"
#include "podio/EventStore.h"

// STL
#include <iostream>
#include <vector>
#include <sstream>
#include <limits>

int main(int, char**){
  auto store = podio::EventStore();
  podio::ROOTNTupleWriter writer("example_rntuple.root", &store);
  write(store, writer);



}
