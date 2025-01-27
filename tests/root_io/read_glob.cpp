#include "podio/Reader.h"
// #include "podio/ROOTReader.h"
#if PODIO_ENABLE_DATASOURCE
  #include "podio/DataSource.h"
#endif

#define ASSERT(condition, msg)                                                                                         \
  if (!(condition)) {                                                                                                  \
    throw std::runtime_error(msg);                                                                                     \
  }

int main() {
  const auto pattern = "example_frame_?.root";
  const auto expected_events = 20;
  const auto reader = podio::makeReader(pattern);
  reader.getEvents();
  ASSERT((reader.getEntries(podio::Category::Event) == expected_events), "Reader read invalid number of events");
#if PODIO_ENABLE_DATASOURCE
  auto rdf = podio::CreateDataFrame(pattern);
  ASSERT(rdf.Count().GetValue() == expected_events, "DataSource read invalid number of events");
#endif // PODIO_ENABLE_DATASOURCE
}
