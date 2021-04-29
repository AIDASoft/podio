#include "write_test.h"

#include "podio/EventStore.h"
#include "podio/SIOWriter.h"

int main(int, char**) {
  podio::EventStore store;
  podio::SIOWriter writer("example.sio", &store);

  write(store, writer);
}
