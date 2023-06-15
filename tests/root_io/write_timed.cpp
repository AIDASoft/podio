#include "podio/BenchmarkRecorder.h"
#include "podio/ROOTWriter.h"
#include "podio/TimedWriter.h"
#include "write_test.h"

int main() {
  podio::benchmark::BenchmarkRecorder recorder("write_benchmark_root.root");
  podio::EventStore store;
  podio::TimedWriter<podio::ROOTWriter> writer(recorder, "example_timed.root", &store);

  write(store, writer);

  return 0;
}
