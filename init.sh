
platform='unknown'
sw_afs=0
unamestr=`uname`

export PODIO=$PWD/install

if [[ "$unamestr" == 'Linux' ]]; then
    platform='Linux'
    if [[ -d /afs/cern.ch/sw/lcg ]] && [[ `dnsdomainname` = 'cern.ch' ]] ; then
	#should check domain to make sure we're at CERN
	#or is this software available somewhere in Lyon? 
	sw_afs=1
	export PATH=/afs/cern.ch/sw/lcg/contrib/CMake/2.8.9/Linux-i386/bin:${PATH}
	source /afs/cern.ch/sw/lcg/contrib/gcc/4.9.3/x86_64-slc6/setup.sh
	source /afs/cern.ch/exp/fcc/sw/0.5/LCG_80/ROOT/6.04.06/x86_64-slc6-gcc49-opt/bin/thisroot.sh
	source /afs/cern.ch/sw/lcg/releases/LCG_80/Python/2.7.9.p1/x86_64-slc6-gcc49-opt/Python-env.sh
	source /afs/cern.ch/sw/lcg/releases/LCG_82/pytools/1.9_python2.7/x86_64-slc6-gcc49-opt/pytools-env.sh
	export CMAKE_PREFIX_PATH=/afs/cern.ch/sw/lcg/releases/gtest/1.7.0-4f83b/x86_64-slc6-gcc49-opt/
	echo cmake and root taken from /afs/cern.ch/sw/lcg
    fi
    export LD_LIBRARY_PATH=$PODIO/tests:$PODIO/lib:$PODIO/examples:$LD_LIBRARY_PATH
elif [[ "$unamestr" == 'Darwin' ]]; then
    platform='Darwin'
    export DYLD_LIBRARY_PATH=$PODIO/tests:$PODIO/lib:$PODIO/examples:$DYLD_LIBRARY_PATH
fi
echo platform detected: $platform
export PYTHONPATH=$PODIO/python:$PYTHONPATH
