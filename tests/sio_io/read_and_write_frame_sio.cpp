#include "read_and_write_frame.h"

#include "podio/SIOFrameReader.h"
#include "podio/SIOFrameWriter.h"

int main() {
  return rewrite_frames<podio::SIOFrameReader, podio::SIOFrameWriter>("example_frame.sio", "rewritten_frame.sio") +
      read_rewritten_frames<podio::SIOFrameReader>("rewritten_frame.sio");
}
