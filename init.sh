platform='unknown'
unamestr=`uname`

if [ -z "$FCCEDM" ]; then
    export PODIO=$PWD/install
    echo "PODIO is unset, setting to $PODIO"
fi

if [[ "$unamestr" == 'Linux' ]]; then
    platform='Linux'
    if [[ -d /cvmfs/sft.cern.ch/lcg ]]; then
        source /cvmfs/sft.cern.ch/lcg/views/LCG_88/x86_64-slc6-gcc49-opt/setup.sh
        echo cmake and root taken from /cvmfs/sft.cern.ch/lcg
        export PATH=$PATH:/cvmfs/sft.cern.ch/lcg/contrib/llvm/3.9.0/x86_64-slc6-gcc49-opt/bin
    fi
    export LD_LIBRARY_PATH=$PODIO/tests:$PODIO/lib:$PODIO/tests:$LD_LIBRARY_PATH
elif [[ "$unamestr" == 'Darwin' ]]; then
    platform='Darwin'
    export DYLD_LIBRARY_PATH=$PODIO/tests:$PODIO/lib:$PODIO/tests:$DYLD_LIBRARY_PATH
fi
echo platform detected: $platform
export PYTHONPATH=$PODIO/python:$PYTHONPATH
