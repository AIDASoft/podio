#!/bin/bash

source /cvmfs/sft.cern.ch/lcg/views/${VIEW}/setup.sh

cd /Package
source init.sh
mkdir build install
cd build
cmake -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_CXX_STANDARD=17  -DCMAKE_CXX_FLAGS=" -fdiagnostics-color=always " -G Ninja .. && \
ninja -k0 && \
ninja install && \
ctest --output-on-failure
