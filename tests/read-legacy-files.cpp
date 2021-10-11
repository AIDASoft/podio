#include "read_test.h"
#include "podio/ROOTReader.h"

int main(){
  auto reader = podio::ROOTReader();
  reader.openFile("https://key4hep.web.cern.ch/testFiles/podio/v00-13/example.root");

  run_read_test(reader);

  reader.closeFile();
  return 0;
}
