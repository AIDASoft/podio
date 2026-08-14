#include "selected_colls_roundtrip.h"

#include "podio/ArrowReader.h"
#include "podio/ArrowWriter.h"
#include <filesystem>

int main() {
  std::filesystem::remove_all("selected_example_frame.podio_arrow");
  return do_roundtrip<podio::ArrowReader, podio::ArrowWriter>("example_frame.podio_arrow", "selected_example_frame.podio_arrow");
}
