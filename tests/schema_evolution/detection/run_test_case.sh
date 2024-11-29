#!/usr/bin/env sh

set -x


# Script to run a single test case for the schema evolution checking script. The
# names of the schema input files are determined automatically from the test
# case.

category=$(echo ${1} | awk -F':' '{print $1}')
test_case=$(echo ${1} | awk -F':' '{print $2}')

old_schema=${PODIO_BASE}/tests/schema_evolution/detection/${category}/dm_${test_case}_old.yaml
new_schema=${PODIO_BASE}/tests/schema_evolution/detection/${category}/dm_${test_case}_new.yaml

${PODIO_BASE}/python/podio_schema_evolution.py ${new_schema} ${old_schema}
