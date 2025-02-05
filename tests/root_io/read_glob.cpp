#include "podio/Reader.h"
#include "podio/utilities/Glob.h"
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
  // standalone globbing

  ASSERT(podio::utils::is_glob_pattern(pattern), "Failed to recognize glob pattern");
  auto files = podio::utils::expand_glob(pattern);
  ASSERT(files.size() == 2, "Glob expanded to a wrong number of files");
  ASSERT(files[0] == "example_frame_0.root", "Glob expanded to an unexpected file");
  ASSERT(files[1] == "example_frame_1.root", "Glob expanded to an unexpected file");
  {
    // externally resolved glob
    const auto reader = podio::makeReader(files);
    ASSERT((reader.getEvents() == expected_events), "Reader read invalid number of events");
#if PODIO_ENABLE_DATASOURCE
    auto rdf = podio::CreateDataFrame(files);
    ASSERT(rdf.Count().GetValue() == expected_events, "DataSource read invalid number of events");
#endif // PODIO_ENABLE_DATASOURCE
  }
  {
    // implicit globbing
    const auto reader = podio::makeReader(pattern);
    ASSERT((reader.getEvents() == expected_events), "Reader read invalid number of events");
#if PODIO_ENABLE_DATASOURCE
    auto rdf = podio::CreateDataFrame(pattern);
    ASSERT(rdf.Count().GetValue() == expected_events, "DataSource read invalid number of events");
#endif // PODIO_ENABLE_DATASOURCE
  }
}
