#include "read_and_write_frame.h"

#include "podio/ArrowReader.h"
#include "podio/ArrowWriter.h"
#include <filesystem>

int main() {
  std::filesystem::remove_all("rewritten_frame.podio_arrow");
  return rewrite_frames<podio::ArrowReader, podio::ArrowWriter>("example_frame.podio_arrow", "rewritten_frame.podio_arrow") +
      read_rewritten_frames<podio::ArrowReader>("rewritten_frame.podio_arrow");
}
