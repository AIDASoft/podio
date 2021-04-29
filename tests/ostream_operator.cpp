#include "datamodel/ExampleForCyclicDependency1.h"
#include "datamodel/ExampleForCyclicDependency2.h"
#include "datamodel/ExampleMC.h"
#include "datamodel/ExampleReferencingType.h"

#include <iostream>

// When using CTest for unit testing it is enough for this test to eventually
// segfault
int main(int, char**) {
  ExampleMC mcp1;
  ExampleMC mcp2;

  mcp1.adddaughters(mcp2);
  mcp2.addparents(mcp1);

  // This will lead to an infinite loop and a core dump eventually
  std::cout << mcp1 << std::endl;

  // Make sure everything still works if the relation is not of the same type
  ExampleForCyclicDependency1 cyc1;
  ExampleForCyclicDependency2 cyc2;

  cyc1.ref(cyc2);
  cyc2.ref(cyc1);

  std::cout << cyc1 << cyc2 << std::endl;

  // Non-cyclical references
  ExampleReferencingType ref1;
  ExampleReferencingType ref2;
  ExampleReferencingType ref3;

  ref1.addRefs(ref2);
  ref2.addRefs(ref3);

  std::cout << ref1 << std::endl;

  return 0;
}
