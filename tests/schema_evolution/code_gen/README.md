# *Code generation* tests for schema evolution

This folder contains small and targeted test cases to ensure that code
generation related to schema evolution works as expected. The goal is to have
rather small test cases that focus on one specific (or a limited set of related)
schema changes to ensure that the related code generation works as intended.

## Setup and test structure
Each test case is assumed to live in its own subdirectory which will also serve
as the name of the test case. In this directory three files are assumed to be
present:
- `old.yaml`: Defining the old version of the schema
- `new.yaml`: Defining the new version of the schema
- `check.cpp`: Containing the test code that can write data in the old format
  and read them in the new format. This file will be compiled once with
  `-DPODIO_SCHEMA_EVOLUTION_TEST_WRITE` and once with
  `-DPODIO_SCHEMA_EVOLUTION_TEST_READ` which allows to have different versions
  of a datamodel available without having to write a lot of the boiler plate
  twice.

The cmake function `ADD_SCHEMA_EVOLUTION_TEST` takes care of generating the
necessary datamodels and compiling and linking all binaries that are required.
It also ensures that the binaries (most importantly dictionaries) are physically
separated such that test runtime environments can be constructed accordingly.

If the `RNTUPLE` argument / flag is passed to `ADD_SCHEMA_EVOLUTION_TEST` the
test will use the `RNTuple{Writer,Reader}` instead of the default
`ROOT{Writer,Reader}`.

To avoid re-generating the datamodel again for the same check with different
backends, the `NO_GENERATE_MODELS` flag can be passed to
`ADD_SCHEMA_EVOLUTION_TEST`.
### Helper macros for `check.cpp`
In order to remove some of the boiler plate that is necessary to write and read
a collection of a certain type, the `check_base.h` file defines helper macros
(that obey the two available compile options). These are `WRITE_AS`,
`READ_AS` and `ASSERT_EQUAL`. The typical content of a `check.cpp` file will
look something like this

```cpp
#include "check_base.h"

#include "datamodel/TestTypeCollection.h"  // The type that undergoes schema evolution

int main() {
  WRITE_AS(TestTypeCollection, {
    // In this code block a collection of type TestTypeCollection
    // will be available as coll.
    // Additionally, a single element of name elem is present
    elem.s(42);  // Set the s-member to 42
  });
  READ_AS(TestTypeCollection, {
    // In this code block a collection of type TestTypeCollection
    // has been read from the provided file and is available as coll.
    // Additionally, the first element of this collection is present
    // as elem
    ASSERT_EQUAL(elem.s(), 42, "Member s does not have the expected value");
  });
}
```

### Additional technical details

For developers who have to touch the internals of the whole machinery a few
additional information bits about the expectations / assumptions:
- `{WRITE,READ}_AS` assume that they can cobble together a filename via
  `TEST_CASE` and `FILE_SUFFIX` pre-processor string literals. The former is
  injected via `target_compile_definitions` in `ADD_SCHEMA_EVOLUTION_TEST` the
  latter is defined in `check_base.h` directly depending on other pre-processor
  flag definitions
- `{WRITE,READ}_AS` are only defined as non-empty macros if
  `PODIO_SCHEMA_EVOLUTION_TEST_{WRITE,READ}` are defined respectively. This is
  what makes it possible to re-use the same source file.
- Both macros are implemented via `{WRITE,READ}_WITH` macros respectively. These
  are slightly more generic and take a writer or reader type as well as a
  filename as argument to setup the basic writer or reader and a corresponding
  Frame called `event`.
- If `RNTUPLE` is passed to `ADD_SCHEMA_EVOLUTION_TEST`,
  `PODIO_SCHEMA_EVOLUTION_RNTUPLE` will also be injected via
  `target_compile_definitions`
