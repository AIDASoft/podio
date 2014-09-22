albers
======

To build and install this package, do (on lxplus):

Preparing the environment - CMake, gcc, ROOT

    export PATH=/afs/cern.ch/sw/lcg/contrib/CMake/2.8.9/Linux-i386/bin:${PATH}
    source /afs/cern.ch/sw/lcg/contrib/gcc/4.8.1/x86_64-slc6/setup.sh
    source /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.20/x86_64-slc6-gcc48-opt/root/bin/thisroot.sh

after setting up a separate build area, the build can be triggered w/

    cmake -DCMAKE_INSTALL_PREFIX=<destination> <path_to_sources>
    make
    make install

either one uses the examples from the build area or from the install area
for using from the install area one has to set the LD_LIBRARY_PATH to point to

    <destination>/examples
    <destination>/lib

The examples are for creating a file "example.root"

    write

And reading it again

    read

if you want to invoke the data model creator use python/albers_class_generator.py
and look into examples/datalayout.yaml for inspiration
