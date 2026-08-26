#include "read_python_frame.h"

#include "podio/ArrowReader.h"

int main() {
  return read_frame<podio::ArrowReader>("example_frame_with_py.podio_parquet");
}
