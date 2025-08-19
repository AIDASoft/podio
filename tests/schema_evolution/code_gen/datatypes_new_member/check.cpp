#include "datamodel/ExampleHitCollection.h"

#include "check_base.h"

#include "podio/ROOTReader.h"
#include "podio/ROOTWriter.h"

int main() {
  WRITE_WITH(podio::ROOTWriter, "datatypes_new_member.root", ExampleHitCollection, {
    elem.x(1.23);
    elem.y(1.23);
    elem.z(1.23);
    elem.cellID(0xcaffee);
  });
  READ_WITH(podio::ROOTReader, "datatypes_new_member.root", ExampleHitCollection,
            {
              ASSERT_EQUAL(elem.t(), 0, "New datatype member variable not 0 initialized");
              ASSERT_EQUAL(elem.x(), 1.23, "Member variables unrelated to schema evolution have changed");
              ASSERT_EQUAL(elem.y(), 1.23, "Member variables unrelated to schema evolution have changed");
              ASSERT_EQUAL(elem.z(), 1.23, "Member variables unrelated to schema evolution have changed");
              ASSERT_EQUAL(elem.energy(), 0, "Member variables unrelated to schema evolution have changed");
              ASSERT_EQUAL(elem.cellID(), 0xcaffee, "Member variables unrelated to schema evolution have changed");
            }

  );
}
