#include "datamodel/TestTypeCollection.h"

#include "check_base.h"

int main() {
  WRITE_AS_OLDEST(TestTypeCollection, { elem.s().x = 42; });
  WRITE_AS_OLD(TestTypeCollection, {
    elem.s().x = 42;
    elem.s().y = 53;
  });

  READ_AS_FROM_VERSION(TestTypeCollection, "oldest", {
    ASSERT_EQUAL(elem.s().z, 0, "New component member variable is not zero initialized");
    ASSERT_EQUAL(elem.s().y, 0, "New component member variable is not zero initialized");
    ASSERT_EQUAL(elem.s().x, 42, "Member variable unrelated to schema evolution has changed");
  });

  READ_AS_FROM_VERSION(TestTypeCollection, "old", {
    ASSERT_EQUAL(elem.s().z, 0, "New component member variable is not zero initialized");
    ASSERT_EQUAL(elem.s().y, 53, "Member variable unrelated to schema evolution has changed");
    ASSERT_EQUAL(elem.s().x, 42, "Member variable unrelated to schema evolution has changed");
  });
  return 0;
}
