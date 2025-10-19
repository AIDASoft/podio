#ifdef PODIO_SCHEMA_EVOLUTION_TEST_WRITE
  #include "datamodel/ExampleHitRemovedCollection.h"
#endif
#include "datamodel/ExampleHitStaysCollection.h"

#include "check_base.h"

int main() {
  WRITE_WITH(WriterT, (TEST_CASE FILE_SUFFIX), {
    auto coll = ExampleHitRemovedCollection();
    auto elem = coll.create();
    elem.x(1.23);
    elem.y(1.23);
    elem.z(1.23);
    elem.cellID(0xcaffee);
    event.put(std::move(coll), "ExampleHitRemoved");
    auto coll2 = ExampleHitStaysCollection();
    auto elem2 = coll2.create();
    elem2.t(99);
    event.put(std::move(coll2), "evolution_collection");
  });
#ifdef PODIO_SCHEMA_EVOLUTION_TEST_READ
  auto reader = ReaderT();
  std::cout << "Reading file " << TEST_CASE FILE_SUFFIX << std::endl;
  reader.openFile((TEST_CASE FILE_SUFFIX));
  // TODO: Make this work when reading all collections
  auto event = podio::Frame(reader.readNextEntry(podio::Category::Event, {"evolution_collection"}));
  const auto& coll = event.get<ExampleHitStaysCollection>("evolution_collection");                                                 \
  const auto elem = coll[0];                                                                                       \
  ASSERT_EQUAL(elem.t(), 99, "Member variables unrelated to schema evolution have changed");
#endif
  return 0;
}
