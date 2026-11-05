#include "datamodel/TestTypeCollection.h"

#include "check_base.h"

int main() {
  WRITE_AS_OLDEST(TestTypeCollection, { elem.x(42); });
  WRITE_AS_OLD(TestTypeCollection, {
    elem.x(42);
    elem.y(53);
  });

  READ_AS_FROM_VERSION(TestTypeCollection, "oldest", {
    ASSERT_EQUAL(elem.z(), 0, "New datatype member variable is not zero initialized");
    ASSERT_EQUAL(elem.y(), 0, "New datatype member variable is not zero initialized");
    ASSERT_EQUAL(elem.x(), 42, "Member variable unrelated to schema evolution has changed");
  });

  READ_AS_FROM_VERSION(TestTypeCollection, "old", {
    ASSERT_EQUAL(elem.z(), 0, "New datatype member variable is not zero initialized");
    ASSERT_EQUAL(elem.y(), 53, "Member variable unrelated to schema evolution has changed");
    ASSERT_EQUAL(elem.x(), 42, "Member variable unrelated to schema evolution has changed");
  });
  return 0;
}
