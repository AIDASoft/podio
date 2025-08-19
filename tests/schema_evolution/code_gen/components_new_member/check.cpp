#include "datamodel/TestTypeCollection.h"

#include "check_base.h"

int main() {
  WRITE_AS(TestTypeCollection, {
    elem.s().x = 42;
    elem.s().y = 43;
    elem.s().z = 44;
    elem.s().p = {1, 2, 3, 4};
  });
  READ_AS(TestTypeCollection, {
    ASSERT_EQUAL(elem.s().t, 0, "New component member variable is not zero initialized");
    ASSERT_EQUAL(elem.s().x, 42, "Member variable unrelated to schema evolution has changed");
    ASSERT_EQUAL(elem.s().y, 43, "Member variable unrelated to schema evolution has changed");
    ASSERT_EQUAL(elem.s().z, 44, "Member variable unrelated to schema evolution has changed");
    ASSERT_EQUAL(elem.s().p[0], 1, "Member variable unrelated to schema evolution has changed");
    ASSERT_EQUAL(elem.s().p[1], 2, "Member variable unrelated to schema evolution has changed");
    ASSERT_EQUAL(elem.s().p[2], 3, "Member variable unrelated to schema evolution has changed");
    ASSERT_EQUAL(elem.s().p[3], 4, "Member variable unrelated to schema evolution has changed");
  });
}
