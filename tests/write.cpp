#include "write_test.h"

#include "podio/EventStore.h"
#include "podio/ROOTWriter.h"

int main(int, char**) {
  auto store = podio::EventStore();
  podio::ROOTWriter writer("example.root", &store);
  write(store, writer);

  // start from a clean slate for the second file
  auto store2 = podio::EventStore();
  auto writer2 = podio::ROOTWriter("example1.root", &store2);
  write(store2, writer2);
}
