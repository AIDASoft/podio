# Schema Evolution tests
This folder contains tests for the schema evolution functionality in podio. The
functionality is tested by first writing data with an old schema version and
then reading in with the current schema version.
[`datalayout_new.yaml`](./datalayout_new.yaml) holds the definition of the new
version, using schema version 3, while the
[`datalayout.yaml`](../datalayout.yaml) that is also used for the other I/O
tests is used as the old version (schema version 2).

## Differences between the two schema versions
Since it is not immediately visible from the test code this list contains the
differences between the two schema versions, and also how this evolution is
tested (if it is supported)

| component / datatype | difference from v2 to v3 | purpose of test | tested with |
|--|--|--|--|
| `SimpleStruct` | no `int t` member in v2 | Addition of new members in components | As  member of `ExampleWithArrayComponent` |
| `ExampleHit` | no `double t` member in v1 | Addition of new members in datatypes | Directly via `ExampleHit` |
| `ex2::NamespaceStruct` | renaming of `y` to `y_new` | Renaming of member variables | As member of `ex42::ExampleWithNamespace` |
| `ex42::ExampleWithARelation` | type of `number` member | migration of `float` to `double` | Direcetly via `ex42::ExampleWithARelation` |
