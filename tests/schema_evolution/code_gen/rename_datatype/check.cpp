#ifdef PODIO_SCHEMA_EVOLUTION_TEST_WRITE
  #include <datamodel/OldTestTypeCollection.h>
#else
  #include <datamodel/NewTestTypeCollection.h>
#endif

#include "check_base.h"

int main() {
  WRITE_AS(OldTestTypeCollection, { elem.i(42); });

  READ_AS(NewTestTypeCollection,
          { ASSERT_EQUAL(elem.i(), 42, "Member of renamed type does not have the expected content"); });
}
