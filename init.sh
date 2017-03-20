platform='unknown'
unamestr=`uname`

if [ -z "$FCCEDM" ]; then
    export PODIO=$PWD/install
    echo "PODIO is unset, setting to $PODIO"
fi

if [[ "$unamestr" == 'Linux' ]]; then
    platform='Linux'
    if [[ -d /cvmfs/sft.cern.ch/lcg ]]; then
	#should check domain to make sure we're at CERN
	#or is this software available somewhere in Lyon?
    source /cvmfs/sft.cern.ch/lcg/views/LCG_88/x86_64-slc6-gcc49-opt/setup.sh
	echo cmake and root taken from /cvmfs/sft.cern.ch/lcg
    fi
    export LD_LIBRARY_PATH=$PODIO/tests:$PODIO/lib:$PODIO/tests:$LD_LIBRARY_PATH
elif [[ "$unamestr" == 'Darwin' ]]; then
    platform='Darwin'
    export DYLD_LIBRARY_PATH=$PODIO/tests:$PODIO/lib:$PODIO/tests:$DYLD_LIBRARY_PATH
fi
echo platform detected: $platform
export PYTHONPATH=$PODIO/python:$PYTHONPATH
