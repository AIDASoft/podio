# Building a new data model library with CMake

PODIO exposes multiple CMake macros to enable the building of data model libraries.
For a package developer tutorial on CMake, please have a look [here](https://hsf-training.github.io/hsf-training-cmake-webpage/). PODIO follows the same conventions used therein.

## Exported CMake functions and macros
The file [cmake/podioMacros.cmake](https://github.com/AIDASoft/podio/blob/master/cmake/podioMacros.cmake) provides the following functions for external use:

  1. `PODIO_GENERATE_DATAMODEL` - generate the data model from provided yaml file
  2. `PODIO_ADD_DATAMODEL_CORE_LIB` - compile the generated data model
  3. `PODIO_ADD_ROOT_IO_DICT` - configure ROOT I/O backend and interface to Python
  4. `PODIO_ADD_SIO_IO_BLOCKS` - configure SIO backend

Their exact parameters and functionality are documented in the file mentioned above. Below a few examples.

## A simple example
After a proper `find_package(podio REQUIRED)`, the C++ code of the new data model `newdm` can be created and compiled via

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
PODIO_ADD_SIO_IO_BLOCKS(newdm "${headers}" "${sources}")
```

For a complete example, please have a look at [EDM4hep](https://github.com/key4hep/EDM4hep/blob/main/edm4hep/CMakeLists.txt)

## More advanced data model generation
PODIO provides additional options within the `PODIO_GENERATE_DATAMODEL` macro that allow developers to fine-tune how data models are generated and compiled:

  1. `UPSTREAM_EDM` - use an existing EDM an upstream dependency. This is useful when your data model extends or builds on top of another one, such as using EDM4hep in [EDM4eic](https://github.com/eic/EDM4eic/blob/7a627488cde0bc4e1863d3fd6bd3396fd30c7296/CMakeLists.txt#L44-L48).
  2. `DEPENDS` - tells CMake that some files used to build the data model are not generated during the current build but are still required for compilation, like in [EDM4hep](https://github.com/key4hep/EDM4hep/blob/ac83112d66f7aa3b9d4eb5859c19b987feab3ce5/edm4hep/CMakeLists.txt#L6-L8)
  3. `VERSION` - specifies how to inject the version of the datamodel (it is separate from the schema version).
  4. `OUTPUT_FOLDER` - the folder in which the output files should be placed. Defaults to `${CMAKE_CURRENT_SOURCE_DIR}` if not set.
  5. `IO_BACKEND_HANDLERS` - the I/O backend handlers that should be generated. The list is passed directly to podio_class_generator.py and validated there. Default is `ROOT`

## Example usage of Advanced options

```cmake
# Generate the C++ code from the YAML definition with an upstream EDM, dependencies, version, output folder, and I/O handlers
PODIO_GENERATE_DATAMODEL(newdm newdm.yaml headers sources 
    UPSTREAM_EDM "EDM4hep:edm4hep"
    DEPENDS "external_file.yaml;another_dependency.yaml"
    VERSION "1.0"
    OUTPUT_FOLDER ${CMAKE_BINARY_DIR}/generated_sources
    IO_BACKEND_HANDLERS "ROOT;SIO"
)

# Compile the core data model shared library (no I/O)
PODIO_ADD_DATAMODEL_CORE_LIB(newdm "${headers}" "${sources}")

# Generate and compile the ROOT I/O dictionary for the default I/O system
PODIO_ADD_ROOT_IO_DICT(newdmDict newdm "${headers}" src/selection.xml)

# Compile the SIOBlocks shared library for the SIO backend
PODIO_ADD_SIO_IO_BLOCKS(newdm "${headers}" "${sources}")
```
Another example can be found [here](https://github.com/AIDASoft/podio/blob/64e87e153e8217375a167123d3567a765935333a/tests/schema_evolution/CMakeLists.tx).

You can explore all available options [here](https://github.com/AIDASoft/podio/blob/cf147a00e00bfbedb2f66330b837679c0c81f6b2/cmake/podioMacros.cmake#L110-L135).
