#!/usr/bin/env bash

set -euo pipefail

# Create a temporay folder to get the data to
PODIO_TEST_INPUT_DATA_DIR=$(mktemp -d -p $(pwd) test_input_data_XXXXXXXX)
export PODIO_TEST_INPUT_DATA_DIR

# Get a legacy file with the oldest version that we still support
cd ${PODIO_TEST_INPUT_DATA_DIR}
mkdir v00-13 && cd v00-13
wget https://key4hep.web.cern.ch:443/testFiles/podio/v00-13/example.root > /dev/null 2>&1

# Announce where we store variables to the outside
echo -n ${PODIO_TEST_INPUT_DATA_DIR}
