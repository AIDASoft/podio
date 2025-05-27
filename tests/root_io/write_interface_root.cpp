#include "write_interface.h"

#include <string>

int main(int argc, char* argv[]) {

  // Check if the user provided a filename as an argument
  std::string outputFile = "example_frame_interface.root";
  if (argc == 2) {
    outputFile = argv[1];
  } else if (argc > 2) {
    std::cerr << "Usage: " << argv[0] << " [output_file_name]" << std::endl;
    return 1;
  }

  auto writer = podio::makeWriter(outputFile);
  write_frames(writer);

  return 0;
}
