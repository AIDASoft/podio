# podio

## Preparing the environment

### On lxplus

To build and install this package, do:

    source init.sh

### On Mac OS

Assuming the path to your version of ROOT is `<root_path>`, do:

    source <root_path>/bin/thisroot.sh

Set up python. We advise to use the version of python that comes with Mac OS. This version should be 2.7.X

    python --version
    > Python 2.7.5

Check that the yaml python module is available

    python
    >>> import yaml

If the import goes fine (no message), you're all set. If not, you need to install yaml. For that, you need to:

1- install the C++ yaml library, which is used by the python module. The easiest way to do that is to use homebrew (install homebrew if you don't have it yet)

    brew install libyaml

2- install the python yaml module (first install pip if you don't have it yet)

    pip install yaml

Check that you can now import the yaml module in python.

Finally, set your environment:

    source init_macos.sh


## Compiling

after setting up a separate build and install area, the build can be triggered w/

    mkdir build
    mkdir install
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=../install ..
    make -j 4 install

## Running

The examples are for creating a file "example.root"

    ../install/examples/write

And reading it again

    ../install/examples/read


## Modifying the data model

If you want to invoke the data model creator use `python/podio_class_generator.py` and look into `tests/datalayout.yaml` for inspiration

## Running tests
After compilation you can run rudimentary tests with:

    make test

### Unit tests
In order to run the unit tests you have to enable them at configuration time with the flag `tests`. Note that GTest is needed. On lxplus the necessary environment is set up with the `init.sh`. On MacOS, you'll have to point cmake to your installation if it is local, see the option in braces below assuming you installed it in `<g_test_path>`.

To configure with unit tests enabled, do:

    cmake -DCMAKE_INSTALL_PREFIX=../install -Dtests=1 (-DCMAKE_PREFIX_PATH=<g_test_path>/install) ..
    make test

