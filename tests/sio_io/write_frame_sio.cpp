#include "datamodel/ExampleWithARelation.h"
#include "write_frame.h"

#include "podio/SIOWriter.h"

#include "podio/detail/AssociationSIOBlock.h"

const static auto foo = podio::AssociationSIOBlock<ExampleMC, ex42::ExampleWithARelation>{};

int main(int, char**) {
  write_frames<podio::SIOWriter>("example_frame.sio");
  return 0;
}
