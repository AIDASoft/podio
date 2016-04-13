# - Config file for the podio package
# It defines the following variables
#  podio_INCLUDE_DIRS - include directories
#  podio_LIBRARIES    - libraries to link against
#  podio_LIBRARY_DIR  - podio library dir
#  podio_BINARY_DIR   - binary directory

# Compute paths
get_filename_component(podio_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

get_filename_component(podio_INCLUDE_DIRS "${podio_CMAKE_DIR}/../include" ABSOLUTE)
get_filename_component(podio_BINARY_DIR "${podio_CMAKE_DIR}/../bin" ABSOLUTE)
get_filename_component(podio_LIBRARY_DIR "${podio_CMAKE_DIR}/../lib" ABSOLUTE)


FIND_LIBRARY( podio_LIBRARIES NAMES podio PATHS
             ${podio_LIBRARY_DIR}
             NO_DEFAULT_PATH
        )

INCLUDE( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( podio DEFAULT_MSG podio_LIBRARY_DIR podio_INCLUDE_DIRS podio_LIBRARIES )

ADD_LIBRARY(examplelibrary SHARED IMPORTED)
