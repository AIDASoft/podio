#include "write_test.h"
#include "podio/SIOWriter.h"

int main(int argc, char* argv[]){
  write<podio::SIOWriter>("example.sio");
}
