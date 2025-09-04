#include "datamodel/TestTypeCollection.h"

#include "check_base.h"

int main() {
  WRITE_AS(nsp::TestTypeCollection, { elem.oldEnergy(3.14); });
  READ_AS(nsp::TestTypeCollection,
          { ASSERT_EQUAL(elem.energy(), 3.14, "Renamed datatype member does not have the expected value"); })
}
