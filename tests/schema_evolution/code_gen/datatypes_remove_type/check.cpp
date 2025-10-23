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
  READ_AS(ExampleHitStaysCollection,
          { ASSERT_EQUAL(elem.t(), 99, "Member variables unrelated to schema evolution have changed"); });
}
