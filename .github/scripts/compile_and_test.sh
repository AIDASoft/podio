#!/bin/bash

SIO_HANDLERS=${USE_SIO:-OFF}

# Depending on whether we need SIO or not setup the environment accordingly
if [ ${SIO_HANDLERS} = "OFF" ]; then
    source /cvmfs/sft.cern.ch/lcg/views/${VIEW}/setup.sh
else
    source /cvmfs/clicdp.cern.ch/iLCSoft/${VIEW}/init_ilcsoft.sh
    # Currently we need to "hack" the datalayout to actually generate SIO block handlers
    # TODO: handle this properly once the how is settled (see #131 for a discussion)
    sed -i 's/\(IOHandlers:\) .*/\1 ["ROOT", "SIO"]/' /Package/tests/datalayout.yaml
fi

cd /Package
source init.sh
mkdir build install
cd build
cmake -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_CXX_STANDARD=17 -DSIO_HANDLERS=${SIO_HANDLERS} -DCMAKE_CXX_FLAGS=" -fdiagnostics-color=always " -G Ninja .. && \
ninja -k0 && \
ninja install && \
ctest --output-on-failure
