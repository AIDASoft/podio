export PATH=/afs/cern.ch/sw/lcg/contrib/CMake/2.8.9/Linux-i386/bin:${PATH}
source /afs/cern.ch/sw/lcg/contrib/gcc/4.9.3/x86_64-slc6/setup.sh
source /afs/cern.ch/exp/fcc/sw/0.5/LCG_80/ROOT/6.04.06/x86_64-slc6-gcc49-opt/bin/thisroot.sh
source /afs/cern.ch/sw/lcg/releases/LCG_80/Python/2.7.9.p1/x86_64-slc6-gcc49-opt/Python-env.sh
source /afs/cern.ch/sw/lcg/releases/LCG_82/pytools/1.9_python2.7/x86_64-slc6-gcc49-opt/pytools-env.sh
export CMAKE_PREFIX_PATH=/afs/cern.ch/sw/lcg/releases/gtest/1.7.0-4f83b/x86_64-slc6-gcc49-opt/

export PODIO=$PWD/install
export LD_LIBRARY_PATH=$PODIO/tests:$PODIO/lib:$PODIO/examples:$LD_LIBRARY_PATH
export PYTHONPATH=$PODIO/examples:$PODIO/tests:$PYTHONPATH
