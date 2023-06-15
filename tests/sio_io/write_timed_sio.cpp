#include "podio/BenchmarkRecorder.h"
#include "podio/EventStore.h"
#include "podio/SIOWriter.h"
#include "podio/TimedWriter.h"
#include "write_test.h"

int main() {
  podio::benchmark::BenchmarkRecorder recorder("write_benchmark_sio.root");
  podio::EventStore store;
  podio::TimedWriter<podio::SIOWriter> writer(recorder, "example_timed.sio", &store);

  write(store, writer);

  return 0;
}
