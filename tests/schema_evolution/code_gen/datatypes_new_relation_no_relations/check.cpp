#include "datamodel/ExampleHitCollection.h"

#include "check_base.h"

int main() {
  WRITE_AS(ExampleHitCollection, {
    elem.num(5);
    elem.addvalues(7);
    elem.addvalues(8);
    auto other = coll.create();
    other.num(6);
  });

  READ_AS(ExampleHitCollection, {
    ASSERT_EQUAL(coll.size(), 2, "Collection with a new relation should be readable");
    ASSERT_EQUAL(elem.num(), 5, "Member variables unrelated to schema evolution have changed");
    ASSERT_EQUAL(elem.values().size(), 2, "Vector members unrelated to schema evolution have changed");
    ASSERT_EQUAL(elem.values()[1], 8, "Vector members unrelated to schema evolution have changed");
    ASSERT_EQUAL(coll[1].num(), 6, "Member variables unrelated to schema evolution have changed");
    for (const auto hit : coll) {
      ASSERT_EQUAL(hit.Neighbours().size(), 0, "New relation should be empty");
    }
  });
}
