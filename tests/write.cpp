#include "write_test.h"
#include "podio/ROOTWriter.h"

int main(int argc, char* argv[]){
  write<podio::ROOTWriter>("example.root");
  write<podio::ROOTWriter>("example1.root");
}
