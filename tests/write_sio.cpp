#include "write_test.h"
#include "podio/SIOWriter.h"
#include "podio/EventStore.h"

int main(int argc, char* argv[]){
  podio::EventStore store;
  podio::SIOWriter writer("example.sio", &store);

  write(store, writer);
}
