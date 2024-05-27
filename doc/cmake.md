# Building a new data model library with CMake

PODIO exposes multiple CMake macros to enable the building of data model libraries.
For a package developer tutorial on CMake, please have a look [here])https://hsf-training.github.io/hsf-training-cmake-webpage/). PODIO follows the same conventions used therein.

## Exported CMake functions and macros
The file [cmake/podioMacros.cmake](https://github.com/AIDASoft/podio/blob/master/cmake/podioMacros.cmake) provides the following functions for external use:

  1. `PODIO_GENERATE_DATAMODEL` - generate the data model from provided yaml file
  2. `PODIO_ADD_DATAMODEL_CORE_LIB` - compile the generated data model
  3. `PODIO_ADD_ROOT_IO_DICT` - configure ROOT I/O backend and interface to Python
  4. `PODIO_ADD_SIO_IO_BLOCKS` - configure SIO backend

Their exact parameters and functionality are documented in the file mentioned above. Below a few examples.

## A simple example
After a proper `find_package(PODIO)`, the C++ code of the new data model `newdm` can be created and compiled via

```cmake
# generate the c++ code from the yaml definition
PODIO_GENERATE_DATAMODEL(newdm newdm.yaml headers sources)

# compile the core data model shared library (no I/O)
PODIO_ADD_DATAMODEL_CORE_LIB(newdm "${headers}" "${sources}")

# generate and compile the ROOT I/O dictionary for the default I/O system
PODIO_ADD_ROOT_IO_DICT(newdmDict newmdm "${headers}" src/selection.xml)
```
For an explicit choice of I/O backends one needs to add the `IO_BACKEND_HANDLERS` parameter and then generate and compile the corresponding backends, like shown below for the ROOT and SIO backends:

```cmake
# generate the c++ code from the yaml definition
PODIO_GENERATE_DATAMODEL(newdm newdm.yaml headers sources IO_BACKEND_HANDLERS "ROOT;SIO")

# compile the core data model shared library (no I/O)
PODIO_ADD_DATAMODEL_CORE_LIB(newdm "${headers}" "${sources}")

# generate and compile the ROOT I/O dictionary for the default I/O system
PODIO_ADD_ROOT_IO_DICT(newdmDict newmdm "${headers}" src/selection.xml)

# compile the SIOBlocks shared library for the SIO backend
PODIO_ADD_SIO_IO_BLOCKS(edm4hep "${headers}" "${sources}")
```

For a complete example, please have a look at [EDM4hep](https://github.com/key4hep/EDM4hep/blob/main/edm4hep/CMakeLists.txt)

## More advanced data model generation
