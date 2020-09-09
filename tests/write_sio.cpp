#include "write_test.h"
#include "podio/SIOWriter.h"

#include "datamodel/EventInfoSIOBlock.h" // here to get the factory working properly

int main(int argc, char* argv[]){
  write<podio::SIOWriter>("example.sio");
  write<podio::SIOWriter>("example1.sio");
}
