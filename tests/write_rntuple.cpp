
#include "podio/ROOTNTupleWriter.h"
#include "podio/EventStore.h"

// skips some parts of the write test 
// known to fail with some root versions
// (16bit fixed with ints cannot be written with rntuple prior to ROOT v6.26)
#define RNTUPLE_WRITE_TEST

#include "write_test.h"

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
