#include "datamodel/MutableExampleMC.h"
#include "datamodel/MutableExampleForCyclicDependency1.h"
#include "datamodel/MutableExampleForCyclicDependency2.h"
#include "datamodel/MutableExampleReferencingType.h"

#include <iostream>

// When using CTest for unit testing it is enough for this test to eventually
// segfault
int main(int, char**)
{
  MutableExampleMC mcp1;
  MutableExampleMC mcp2;

  mcp1.adddaughters(mcp2);
  mcp2.addparents(mcp1);

  // This will lead to an infinite loop and a core dump eventually
  std::cout << mcp1 << std::endl;

  // Make sure everything still works if the relation is not of the same type
  MutableExampleForCyclicDependency1 cyc1;
  MutableExampleForCyclicDependency2 cyc2;

  cyc1.ref(cyc2);
  cyc2.ref(cyc1);

  std::cout << cyc1 << cyc2 << std::endl;

  // Non-cyclical references
  MutableExampleReferencingType ref1;
  MutableExampleReferencingType ref2;
  ExampleReferencingType ref3;

  ref1.addRefs(ref2);
  ref2.addRefs(ref3);

  std::cout << ref1 << std::endl;

  return 0;
}
