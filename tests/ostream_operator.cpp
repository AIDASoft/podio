#include "datamodel/ExampleForCyclicDependency1Collection.h"
#include "datamodel/ExampleForCyclicDependency2Collection.h"
#include "datamodel/ExampleMCCollection.h"
#include "datamodel/ExampleReferencingTypeCollection.h"

#include <iostream>

// When using CTest for unit testing it is enough for this test to eventually
// segfault
int main(int, char**) {
  ExampleMCCollection mcps{};
  auto mcp1 = mcps.create();
  auto mcp2 = mcps.create();

  mcp1.adddaughters(mcp2);
  mcp2.addparents(mcp1);

  // This will lead to an infinite loop and a core dump eventually
  std::cout << mcp1 << std::endl;

  // Make sure everything still works if the relation is not of the same type
  ExampleForCyclicDependency1Collection cyc1Coll{};
  auto cyc1 = cyc1Coll.create();
  ExampleForCyclicDependency2Collection cyc2Coll{};
  auto cyc2 = cyc2Coll.create();

  cyc1.ref(cyc2);
  cyc2.ref(cyc1);

  std::cout << cyc1 << cyc2 << std::endl;

  // Non-cyclical references
  ExampleReferencingTypeCollection refColl;
  auto ref1 = refColl.create();
  auto ref2 = refColl.create();
  auto ref3 = refColl.create();

  ref1.addRefs(ref2);
  ref2.addRefs(ref3);

  std::cout << ref1 << std::endl;

  return 0;
}
