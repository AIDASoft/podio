#include "datamodel/NamespaceTypeCollection.h"

#include "check_base.h"

int main() {
  WRITE_AS(nsp::NamespaceTypeCollection, { elem.intArray({42, 123}); });
  READ_AS(nsp::NamespaceTypeCollection, {
    ASSERT_EQUAL(elem.intArray()[0], 42, "First element of the array should be unchanged");
    ASSERT_EQUAL(elem.intArray()[1], 123, "Second element of the array should be unchanged");
  });
}
