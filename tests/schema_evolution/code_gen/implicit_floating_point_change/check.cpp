#include "datamodel/TestTypeCollection.h"

#include "check_base.h"

int main() {
  WRITE_AS(TestTypeCollection, {
    elem.comp().f = 3.14f;
    elem.energy(1.234);
  });

  READ_AS(TestTypeCollection, {
    ASSERT_EQUAL(elem.comp().f, 3.14f, "Implicit conversion for component members doesn't work");
    ASSERT_EQUAL(elem.energy(), 1.234f, "Implicit conversion for datatype members doesn't work");
  })
}
