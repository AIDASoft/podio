#include "write_frame.h"

#include "podio/SIOWriter.h"

int main(int, char**) {
  write_frames<podio::SIOWriter>("example_frame.sio");
  return 0;
}
