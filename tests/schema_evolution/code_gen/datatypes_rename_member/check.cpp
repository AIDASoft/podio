#include "datamodel/TestTypeCollection.h"

#include "check_base.h"

int main() {
  WRITE_AS(nsp::TestTypeCollection, {
    elem.oldEnergy(3.14);
    elem.unchangedMember(123);
    elem.anotherNonChange(4.3f);
  });
  READ_AS(nsp::TestTypeCollection, {
    ASSERT_EQUAL(elem.energy(), 3.14, "Renamed datatype member does not have the expected value");
    ASSERT_EQUAL(elem.unchangedMember(), 123, "Non renamed member should not change");
    ASSERT_EQUAL(elem.anotherNonChange(), 4.3f, "Non renamed member should not change");
  })
}
