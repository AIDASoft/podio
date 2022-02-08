#include "read_test.h"
#include "podio/SIOReader.h"

int main(){
//  auto reader = podio::SIOReader();
  podio::SIOReader reader; // SIOReader has no copy c'tor ...
  reader.openFile("example.sio");
  if (reader.currentFileVersion() != podio::version::build_version) {
    return 1;
  }

  run_read_test(reader);

  reader.closeFile();
  return 0;
}
