#include "podio/BenchmarkRecorder.h"
#include "podio/SIOReader.h"
#include "podio/TimedReader.h"
#include "read_test.h"

int main() {
  podio::benchmark::BenchmarkRecorder recorder("read_benchmark_sio.root");

  podio::TimedReader<podio::SIOReader> reader(recorder);
  reader.openFile("example_timed.sio");

  run_read_test(reader);

  reader.closeFile();
  return 0;
}
