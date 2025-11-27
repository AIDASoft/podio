#include "datamodel/ExampleMCCollection.h"

#include "check_base.h"

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
  READ_AS_FROM_VERSION(ExampleMCCollection, "old", {
    ASSERT_EQUAL(elem.momentum().x, 3.14, "Unchanged members should be readable");
    ASSERT_EQUAL(elem.momentum().y, 1.23, "Unchanged members should be readable");
    ASSERT_EQUAL(elem.momentum().z, 0.99999, "Unchanged members should be readable");
    ASSERT_EQUAL(elem.PDG(), 11, "Unchanged members should be readable");
  });

  READ_AS_FROM_VERSION(ExampleMCCollection, "oldest", {
    ASSERT_EQUAL(elem.momentum().x, 3.14, "Unchanged members should be readable");
    ASSERT_EQUAL(elem.momentum().y, 1.23, "Unchanged members should be readable");
    ASSERT_EQUAL(elem.momentum().z, 0.99999, "Unchanged members should be readable");
    ASSERT_EQUAL(elem.PDG(), 11, "Unchanged members should be readable");
  });
}
