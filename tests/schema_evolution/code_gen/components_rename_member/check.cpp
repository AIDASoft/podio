#include "datamodel/TestTypeCollection.h"

#include "check_base.h"

int main() {
  WRITE_AS(nsp::TestTypeCollection, { elem.comp().y = 42; });
  READ_AS(nsp::TestTypeCollection,
          { ASSERT_EQUAL(elem.comp().y_new, 42, "Renamed component member does not have the expected value"); })
}
