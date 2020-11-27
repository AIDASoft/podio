#include "read_test.h"
#include "podio/ROOTReader.h"
#include "podio/TimedReader.h"
#include "podio/BenchmarkRecorder.h"

int main() {
  podio::benchmark::BenchmarkRecorder recorder("read_benchmark_root.root");

  podio::TimedReader<podio::ROOTReader> reader(recorder);
  reader.openFile("example_timed.root");

  run_read_test(reader);

  reader.closeFile();
  return 0;
}
