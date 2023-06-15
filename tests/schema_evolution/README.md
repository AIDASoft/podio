# Schema Evolution tests
This folder contains tests for the schema evolution functionality in podio. The
functionality is tested by first writing data with an old schema version and
then reading in with the current schema version.
[`datalayout_old.yaml`](./datalayout_old.yaml) holds the definition of the old
version, using schema version 1, while the
[`datalayout.yaml`](../datalayout.yaml) that is also used for the other I/O
tests is used as the current version (schema version 2).

## Differences between the two schema versions
Since it is not immediately visible from the test code this list contains the
differences between the two schema versions, and also how this evolution is
tested (if it is supported)

| component / datatype | difference from v1 to v2 | purpose of test | tested with |
|--|--|--|--|
| `SimpleStruct` | no `int y` member in v1 | Addition of new members in components | As  member of `ExampleWithArrayComponent` |
| `ExampleHit` | no `double energy` member in v1 | Addition of new members in datatypes | Directly via `ExampleHit` |
