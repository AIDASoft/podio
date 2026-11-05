#include "datamodel/TestTypeCollection.h"

#include "check_base.h"

#ifdef PODIO_SCHEMA_EVOLUTION_TEST_READ
// READ_AS_FROM_VERSION returns on success, so each version needs its own function.
int checkOldest() {
  READ_AS_FROM_VERSION(TestTypeCollection, "oldest", {
    ASSERT_EQUAL(elem.z(), 0, "New datatype member variable is not zero initialized");
    ASSERT_EQUAL(elem.y(), 0, "New datatype member variable is not zero initialized");
    ASSERT_EQUAL(elem.x(), 42, "Member variable unrelated to schema evolution has changed");
  });
}

int checkOld() {
  READ_AS_FROM_VERSION(TestTypeCollection, "old", {
    ASSERT_EQUAL(elem.z(), 0, "New datatype member variable is not zero initialized");
    ASSERT_EQUAL(elem.y(), 53, "Member variable unrelated to schema evolution has changed");
    ASSERT_EQUAL(elem.x(), 42, "Member variable unrelated to schema evolution has changed");
  });
}
#endif

int main() {
  WRITE_AS_OLDEST(TestTypeCollection, { elem.x(42); });
  WRITE_AS_OLD(TestTypeCollection, {
    elem.x(42);
    elem.y(53);
  });

#ifdef PODIO_SCHEMA_EVOLUTION_TEST_READ
  if (checkOldest() || checkOld()) {
    return 1;
  }
#endif
  return 0;
}
