
platform='unknown'
sw_afs=0
unamestr=`uname`

export PODIO=$PWD/install

if [[ "$unamestr" == 'Linux' ]]; then
    platform='Linux'
    if [[ -d /cvmfs/sft.cern.ch ]] && [[ `dnsdomainname` = 'cern.ch' ]] ; then
	#should check domain to make sure we're at CERN
	#or is this software available somewhere in Lyon? 
	sw_afs=1
        source /cvmfs/sft.cern.ch/lcg/views/LCG_84/x86_64-slc6-gcc49-opt/setup.sh
	echo cmake and root taken from /cvmfs/sft.cern.ch
    fi
    export LD_LIBRARY_PATH=$PODIO/tests:$PODIO/lib:$PODIO/tests:$LD_LIBRARY_PATH
elif [[ "$unamestr" == 'Darwin' ]]; then
    platform='Darwin'
    export DYLD_LIBRARY_PATH=$PODIO/tests:$PODIO/lib:$PODIO/tests:$DYLD_LIBRARY_PATH
fi
echo platform detected: $platform
export PYTHONPATH=$PODIO/python:$PYTHONPATH
