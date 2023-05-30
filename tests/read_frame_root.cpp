#include "read_frame.h"
#include "read_frame_auxiliary.h"

#include "podio/ROOTFrameReader.h"

int main() {
  return read_frames<podio::ROOTFrameReader>("example_frame.root") +
      test_frame_aux_info<podio::ROOTFrameReader>("example_frame.root");
}
