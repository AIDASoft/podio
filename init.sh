export PATH=/afs/cern.ch/sw/lcg/contrib/CMake/2.8.9/Linux-i386/bin:${PATH}
source /afs/cern.ch/sw/lcg/contrib/gcc/4.8.1/x86_64-slc6/setup.sh
source /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.20/x86_64-slc6-gcc48-opt/root/bin/thisroot.sh

export ALBERS=$PWD/install
export LD_LIBRARY_PATH=$ALBERS/examples:$ALBERS/lib:$LD_LIBRARY_PATH
export PYTHONPATH=$ALBERS/examples:$PYTHONPATH
