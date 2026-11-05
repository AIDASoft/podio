#include "datamodel/ExampleMCCollection.h"

#include "check_base.h"

#ifdef PODIO_SCHEMA_EVOLUTION_TEST_READ
// READ_AS_FROM_VERSION returns on success, so each version needs its own function.
int checkOld() {
  READ_AS_FROM_VERSION(ExampleMCCollection, "old", {
    ASSERT_EQUAL(elem.momentum().x, 3.14, "Unchanged members should be readable");
    ASSERT_EQUAL(elem.momentum().y, 1.23, "Unchanged members should be readable");
    ASSERT_EQUAL(elem.momentum().z, 0.99999, "Unchanged members should be readable");
    ASSERT_EQUAL(elem.PDG(), 11, "Unchanged members should be readable");
  });
}

int checkOldest() {
  READ_AS_FROM_VERSION(ExampleMCCollection, "oldest", {
    ASSERT_EQUAL(elem.momentum().x, 3.14, "Unchanged members should be readable");
    ASSERT_EQUAL(elem.momentum().y, 1.23, "Unchanged members should be readable");
    ASSERT_EQUAL(elem.momentum().z, 0.99999, "Unchanged members should be readable");
    ASSERT_EQUAL(elem.PDG(), 11, "Unchanged members should be readable");
  });
}
#endif

int main() {
  WRITE_AS_OLDEST(ExampleMCCollection, {
    elem.colorFlow({42, 123});
    elem.momentum({3.14, 1.23, 0.99999});
    elem.PDG(11);
  });
  WRITE_AS_OLD(ExampleMCCollection, {
    elem.colorFlow({42, 123});
    elem.momentum({3.14, 1.23, 0.99999});
    elem.PDG(11);
  });

#ifdef PODIO_SCHEMA_EVOLUTION_TEST_READ
  if (checkOld() || checkOldest()) {
    return 1;
  }
#endif
  return 0;
}
