# *Detection* tests for `podio_schema_evolution.py`

This folder contains small and targeted test cases to ensure that the
`podio_schema_evolution.py` script reliably detects schema changes that are not
trivial. These test cases only deal with detecting these occurrences! Whether the
detected schema evolution steps are supported by podio (yet) are not really
interesting here and they (will be) tested in another place.

## Setup of the tests

In order to allow for some minimal form of automation and to avoid too much
boilerplate the test cases are grouped into categories. Each category then has
several *unit test like* setups where each test covers exactly one schema
change. Each subfolder in this directory represents a category. In each
subfolder there are for each test case (i.e. schema change) exactly two yaml
files with the (minimal) schemas that have an example of the change. To allow
for test automation these yaml files need to follow the naming convention
`dm_<test-case-name>_{old,new}.yaml`, where the `old` yaml file needs to have a
lower `schema_version` than the `new` yaml file.

The `run_test_case.sh` script takes one argument in the form of
`<category-name>:<test-case-name>`. It constructs the necessary file names from
this input and then runs the `podio_schema_evolution.py` script on them.

Finally, in the `CMakeLists.txt` file here, it is simply necessary to add new
test cases to the `should_fail_cases`, `should_succeed_cases` or
`should_warn_cases` lists and they will be automatically picked up.
