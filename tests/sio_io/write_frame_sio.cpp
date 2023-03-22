#include "datamodel/ExampleWithARelation.h"
#include "write_frame.h"

#include "podio/SIOFrameWriter.h"

#include "podio/detail/AssociationSIOBlock.h"

const static auto foo = podio::AssociationSIOBlock<ExampleMC, ex42::ExampleWithARelation>{};

int main(int, char**) {
  write_frames<podio::SIOFrameWriter>("example_frame.sio");
  return 0;
}
