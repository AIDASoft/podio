#!/bin/bash

ENABLE_SIO=${USE_SIO:-OFF}

source init.sh
mkdir build install
cd build
cmake -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_CXX_STANDARD=17 -DENABLE_SIO=${ENABLE_SIO} -DCMAKE_CXX_FLAGS=" -fdiagnostics-color=always -Werror" -G Ninja .. && \
ninja -k0 && \
ninja install && \
ctest --output-on-failure
