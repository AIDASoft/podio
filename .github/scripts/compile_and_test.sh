#!/bin/bash

ENABLE_SIO=${USE_SIO:-OFF}

# Depending on whether we need SIO or not setup the environment accordingly
if [ ${ENABLE_SIO} = "OFF" ]; then
    source /cvmfs/sft.cern.ch/lcg/views/${VIEW}/setup.sh
else
    source /cvmfs/clicdp.cern.ch/iLCSoft/${VIEW}/init_ilcsoft.sh
fi

cd /Package
source init.sh
mkdir build install
cd build
cmake -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_CXX_STANDARD=17 -DENABLE_SIO=${ENABLE_SIO} -DCMAKE_CXX_FLAGS=" -fdiagnostics-color=always " -G Ninja .. && \
ninja -k0 && \
ninja install && \
ctest --output-on-failure
