#include "datamodel/TestTypeCollection.h"

#include "check_base.h"

int main() {
  WRITE_AS_OLDEST(nsp::TestTypeCollection, {
    elem.comp().x = 42;
    elem.comp().y_original = 53;
  });
  WRITE_AS_OLD(nsp::TestTypeCollection, {
    elem.comp().x = 42;
    elem.comp().y = 53;
  });

  READ_AS_FROM_VERSION(nsp::TestTypeCollection, "oldest", {
    ASSERT_EQUAL(elem.comp().y_renamed, 53, "Renamed component member does not have the expected value");
    ASSERT_EQUAL(elem.comp().x, 42, "Member variable unrelated to schema evolution has changed");
  });

  READ_AS_FROM_VERSION(nsp::TestTypeCollection, "old", {
    ASSERT_EQUAL(elem.comp().y_renamed, 53, "Renamed component member does not have the expected value");
    ASSERT_EQUAL(elem.comp().x, 42, "Member variable unrelated to schema evolution has changed");
  });
  return 0;
}
