#!/bin/bash

#Determine which OS you are using
if [ "$(uname)" == "Linux" ]; then
    if [ "$( cat /etc/*-release | grep Scientific )" ]; then
        OS=slc6
    elif [ "$( cat /etc/*-release | grep CentOS )" ]; then
        OS=centos7
    elif [ "$( cat /etc/*-release | grep Bionic )" ]; then
        OS=ubuntu1804
    fi
else
    echo "UNKNOWN OS"
    exit 1
fi

#Determine is you have CVMFS installed
if [ ! -d "/cvmfs" ]; then
    echo "No CVMFS detected, please install it."
    exit 1
fi

if [ ! -d "/cvmfs/sft.cern.ch" ]; then
    echo "No SFT repository detected, please add it."
    exit 1
fi

COMPILER_VERSION="gcc8"
LCG_VERSION="95"

#--------------------------------------------------------------------------------
#     LCG View
#--------------------------------------------------------------------------------

source /cvmfs/sft.cern.ch/lcg/views/LCG_${LCG_VERSION}/x86_64-${OS}-${COMPILER_VERSION}-opt/setup.sh

if [ ${COMPILER_TYPE} == "llvm" ]; then
    source /cvmfs/sft.cern.ch/lcg/contrib/llvm/8.0.0/x86_64-${OS}/setup.sh
fi
