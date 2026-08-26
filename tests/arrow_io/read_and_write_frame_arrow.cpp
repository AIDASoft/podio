#include "read_and_write_frame.h"

#include "podio/ArrowReader.h"
#include "podio/ArrowWriter.h"
#include <filesystem>

int main() {
  std::filesystem::remove_all("rewritten_frame.podio_parquet");
  return rewrite_frames<podio::ArrowReader, podio::ArrowWriter>("example_frame.podio_parquet",
                                                                "rewritten_frame.podio_parquet") +
      read_rewritten_frames<podio::ArrowReader>("rewritten_frame.podio_parquet");
}
