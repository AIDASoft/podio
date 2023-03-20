#include "podio/ROOTNTupleWriter.h"
#include "write_frame.h"

int main(){
  write_frames<podio::ROOTNTupleWriter>("example_rntuple.root");
}
