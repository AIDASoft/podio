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
- `check.cpp`: Containing the test code that can write data in the old(er) formats
  and read them in the new format. This file will be compiled once with
  `-DPODIO_SCHEMA_EVOLUTION_TEST_WRITE` and once with
  `-DPODIO_SCHEMA_EVOLUTION_TEST_READ` which allows to have different versions
  of a datamodel available without having to write a lot of the boiler plate
  twice.
- For tests involving multiple old schemas the canonical datamodel names are
  `oldest.yaml`, `old.yaml` and `new.yaml`.
  - In these cases `check.cpp` will be compiled into a separate binary for all
    versions of the datamodel and an additional `DATAMODEL_VERSION` will be
    injected with an appropriate value.

The cmake function `ADD_SCHEMA_EVOLUTION_TEST` takes care of generating the
necessary datamodels and compiling and linking all binaries that are required.
It also ensures that the binaries (most importantly dictionaries) are physically
separated such that test runtime environments can be constructed accordingly.

If the `RNTUPLE` argument / flag is passed to `ADD_SCHEMA_EVOLUTION_TEST` the
test will use the `RNTuple{Writer,Reader}` instead of the default
`ROOT{Writer,Reader}`.

If the `NO_EVOLUTION_CHECKS` argument / flag is passed the code generation for
the new model will not take into account the old model yaml defintion. Hence,
there will be no checks on whether the evolution is actually supported at the
moment. This makes it possible to write checks that ensure certain behavior even
if the evolution is not supported.

To avoid re-generating the datamodel again for the same check with different
backends, the `NO_GENERATE_MODELS` flag can be passed to
`ADD_SCHEMA_EVOLUTION_TEST`.

## CMake Functions

### GENERATE_DATAMODEL

```cmake
GENERATE_DATAMODEL(test_case model_version [WITH_EVOLUTION] [OLD_VERSIONS version1 version2 ...])
```

Generates the necessary code and builds all required libraries for the specified
datamodel and version. All generated and compiled binary outputs are placed into
a distinct subfolder to allow individual "toggling" of models at test runtime.

**Arguments:**
- `test_case`: The name of the test case
- `model_version`: Which version of the model to generate (old or new)
- `WITH_EVOLUTION` (Optional): Pass an evolution.yaml file to the generation of the model
- `OLD_VERSIONS` (Optional): List of old model versions to pass to OLD_DESCRIPTION

**Requirements:**
- `WITH_EVOLUTION` requires `OLD_VERSIONS` to be specified

### ADD_SCHEMA_EVOLUTION_TEST

```cmake
ADD_SCHEMA_EVOLUTION_TEST(test_case [RNTUPLE] [NO_GENERATE_MODELS] [WITH_EVOLUTION] [OLD_MODELS version1 version2 ...])
```

Adds all the bits and pieces necessary to test a certain schema evolution case,
including datamodel generation (unless disabled) and compilation of executables
to write data in old format(s) and read them back in the new format.

**Arguments:**
- `test_case`: The name of the test case
- `RNTUPLE` (Optional): Use RNTuple backend for testing
- `NO_GENERATE_MODELS` (Optional): Skip generation of datamodels
- `WITH_EVOLUTION` (Optional): Mark this evolution as one that needs intervention
- `OLD_MODELS` (Optional): List of old model versions (defaults to "old")

**Behavior:**
- Creates write executables for each old model version
- Creates a single read executable using the new model
- Sets up appropriate test fixtures and dependencies
- Configures library paths and environment variables for proper model isolation

## Helper macros for `check.cpp`
In order to remove some of the boiler plate that is necessary to write and read
a collection of a certain type, the `check_base.h` file defines helper macros
(that obey the two available compile options). These are `WRITE_AS`,
`READ_AS`, `WRITE_AS_OLDEST`, `WRITE_AS_OLD`, `READ_AS_FROM_VERSION` and
`ASSERT_EQUAL`. For tests with multiple old schema versions, the additional
macros `WRITE_AS_OLDEST`, `WRITE_AS_OLD`, and `READ_AS_FROM_VERSION` allow
writing and reading data from specific schema versions. The typical content
of a `check.cpp` file will look something like this

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

For tests involving multiple schema versions, additional macros are available:

```cpp
// Writing data with specific schema versions
WRITE_AS_OLDEST(TestTypeCollection, {
  // Writes data using the oldest schema version (DATAMODEL_VERSION == 1)
  elem.s(42);
});

WRITE_AS_OLD(TestTypeCollection, {
  // Writes data using the old schema version (DATAMODEL_VERSION == 2)
  elem.s(42);
});

// Reading data from specific schema versions
READ_AS_FROM_VERSION(TestTypeCollection, "oldest", {
  // Reads data from the oldest schema version file
  ASSERT_EQUAL(elem.s(), 42, "Member s does not have the expected value");
});

READ_AS_FROM_VERSION(TestTypeCollection, "old", {
  // Reads data from the old schema version file
  ASSERT_EQUAL(elem.s(), 42, "Member s does not have the expected value");
});
```

### Additional technical details

For developers who have to touch the internals of the whole machinery a few
additional information bits about the expectations / assumptions:
- `{WRITE,READ}_AS[...]` assume that they can cobble together a filename via
  `TEST_CASE` and `FILE_SUFFIX` pre-processor string literals. The former is
  injected via `target_compile_definitions` in `ADD_SCHEMA_EVOLUTION_TEST` the
  latter is defined in `check_base.h` directly depending on other pre-processor
  flag definitions.
- `{WRITE,READ}_AS[...]` are only defined as non-empty macros if
  `PODIO_SCHEMA_EVOLUTION_TEST_{WRITE,READ}` are defined respectively. This is
  what makes it possible to re-use the same source file.
  - A similar trick is used for multiple old versions using the
    `DATAMODEL_VERSION` constant (only defined for `oldest.yaml` and
    `old.yaml`!) This makes `WRITE_AS_OLD[EST]` essentially copies of `WRITE_AS`
    with minor tweaks to the generated filename. However, this rather "clumsy"
    approach was chosen deliberately as it is more readable than a generic
    pre-processor machinery that can stamp out macros as necessary and we don't
    need more than these two versions at the moment.
- Almost all macros are implemented via `{WRITE,READ}_WITH` macros respectively. These
  are slightly more generic and take a writer or reader type as well as a
  filename as argument to setup the basic writer or reader and a corresponding
  Frame called `event`.
- If `RNTUPLE` is passed to `ADD_SCHEMA_EVOLUTION_TEST`,
  `PODIO_SCHEMA_EVOLUTION_RNTUPLE` will also be injected via
  `target_compile_definitions`
