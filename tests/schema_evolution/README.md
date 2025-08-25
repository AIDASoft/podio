# Schema Evolution tests
This folder contains tests for the schema evolution functionality in podio. The
tests cover different aspects of the schema evolution functionality and are
roughly split into disjoint and orthogonal categories:
- Tests in `detection` check whether the `podio_schema_evolution.py` script
  properly detects schema changes and marks possible and impossible ones.
- Tests in `code_gen` on the other hand check whether the expected (supported)
  schema evolution functionalities work as expected.

Both of these folders contain a few more technical details on how the tests
work. These are mainly targetted at other developers as both folders come with a
little mini framework that allows for a rather terse test definition. This
should make it possible to add new tests rather easily. (Assuming that these
test cases can be represented in a similar way as the existing ones and do not
require changes to the underlying machinery).
