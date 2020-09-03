#include "write_test.h"
#include "podio/SIOWriter.h"

int main(int argc, char* argv[]){
  write<podio::SIOWriter>("example.sio");
  write<podio::SIOWriter>("example1.sio");
}
