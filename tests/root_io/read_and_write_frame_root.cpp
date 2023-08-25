#include "read_and_write_frame.h"

#include "podio/ROOTFrameReader.h"
#include "podio/ROOTFrameWriter.h"

int main() {
  return rewrite_frames<podio::ROOTFrameReader, podio::ROOTFrameWriter>("example_frame.root", "rewritten_frame.root") +
      read_rewritten_frames<podio::ROOTFrameReader>("rewritten_frame.root");
}
