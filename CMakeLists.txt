# CMake 3.20 introduced support for C++23.
cmake_minimum_required(VERSION 3.20)

#--- Project name --------------------------------------------------------------
project(podio LANGUAGES CXX)

#--- Version -------------------------------------------------------------------
SET( ${PROJECT_NAME}_VERSION_MAJOR 1 )
SET( ${PROJECT_NAME}_VERSION_MINOR 10 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )

SET( ${PROJECT_NAME}_VERSION  "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}" )

#--- Define basic build settings -----------------------------------------------
# Provides install directory variables as defined for GNU software
include(GNUInstallDirs)

# Define a default build type can be overridden by passing
# ``-DCMAKE_BUILD_TYPE=<type>`` when invoking CMake
if(NOT CMAKE_CONFIGURATION_TYPES)
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo
      CACHE STRING "Choose the type of build, options are: None Release MinSizeRel Debug RelWithDebInfo"
      FORCE
      )
  else()
    set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}"
      CACHE STRING "Choose the type of build, options are: None Release MinSizeRel Debug RelWithDebInfo"
      FORCE
      )
  endif()
endif()

set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")

option(PODIO_SET_RPATH "Link libraries with built-in RPATH (run-time search path)" OFF)
include(cmake/podioBuild.cmake)
podio_set_compiler_flags()
podio_set_rpath()

set(USE_SANITIZER ""
  CACHE STRING "Compile with a sanitizer. Options are Address, Memory, MemoryWithOrigin, Undefined, Thread, Leak, Address;Undefined")
ADD_SANITIZER_FLAGS()
option(FORCE_RUN_ALL_TESTS "Run all the tests even those with known problems" OFF)

option(CLANG_TIDY "Run clang-tidy after compilation." OFF)
ADD_CLANG_TIDY()

#--- Declare options -----------------------------------------------------------
include(CMakeDependentOption)
option(CREATE_DOC        "Whether or not to create doxygen doc target." OFF)
option(ENABLE_ROOT       "Build ROOT I/O support" ON)
option(ENABLE_SIO        "Build SIO I/O support" OFF)
option(ENABLE_ARROW      "Build Arrow I/O support" OFF)
option(ENABLE_PARQUET    "Build Parquet support" OFF)
option(PODIO_RELAX_PYVER "Do not require exact python version match with ROOT" OFF)
cmake_dependent_option(ENABLE_RNTUPLE    "Build with support for the new ROOT NTtuple format" OFF "ENABLE_ROOT" OFF)
cmake_dependent_option(ENABLE_DATASOURCE "Build podio's ROOT DataSource" OFF "ENABLE_ROOT" OFF)
option(PODIO_USE_CLANG_FORMAT "Use clang-format to format the code" OFF)
option(ENABLE_JULIA      "Enable Julia support. When enabled, Julia datamodels will be generated, and Julia tests will run." OFF)

#--- Declare ROOT dependency ---------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(Threads REQUIRED)
if(ENABLE_ROOT)
  list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
  set(root_components_needed RIO Tree)
  if(ENABLE_RNTUPLE)
    list(APPEND root_components_needed ROOTNTuple)
  endif()
  if(ENABLE_DATASOURCE)
    list(APPEND root_components_needed ROOTDataFrame)
  endif()
  # ROOT 6.36 is the first release series supporting C++23 builds.
  find_package(ROOT 6.36 REQUIRED COMPONENTS ${root_components_needed})

  if(NOT ROOT_CXX_STANDARD)
    message(FATAL_ERROR "Could not determine ROOT's C++ standard. podio requires ROOT built with C++23 or newer. To build without ROOT, set ENABLE_ROOT=OFF.")
  elseif(ROOT_CXX_STANDARD LESS 23 OR ROOT_CXX_STANDARD EQUAL 98)
    message(FATAL_ERROR "podio requires ROOT built with C++23 or newer, but ROOT was built with C++${ROOT_CXX_STANDARD}. To build without ROOT, set ENABLE_ROOT=OFF.")
  else()
    message(STATUS "Determined ROOT C++ standard: ${ROOT_CXX_STANDARD}")
  endif()

  list(APPEND PODIO_IO_HANDLERS ROOT)
endif()

# Match ROOT's C++ standard by default so the libraries and Cling agree.
# Without ROOT, default to C++23. Users can override either default with
# -DCMAKE_CXX_STANDARD=<standard>.
if(NOT DEFINED CMAKE_CXX_STANDARD)
  if(ENABLE_ROOT)
    set(CMAKE_CXX_STANDARD "${ROOT_CXX_STANDARD}")
  else()
    set(CMAKE_CXX_STANDARD 23)
  endif()
endif()

if(NOT CMAKE_CXX_STANDARD MATCHES "^[0-9]+$" OR CMAKE_CXX_STANDARD LESS 23 OR CMAKE_CXX_STANDARD EQUAL 98)
  message(FATAL_ERROR "Unsupported C++ standard: ${CMAKE_CXX_STANDARD}. podio requires C++23 or newer")
endif()

if(ENABLE_ROOT AND NOT ROOT_CXX_STANDARD EQUAL CMAKE_CXX_STANDARD)
  message(WARNING "You are trying to build podio with a different C++ standard than ROOT. C++${CMAKE_CXX_STANDARD} was requested but ROOT was built with C++${ROOT_CXX_STANDARD}")
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
message(STATUS "Setting C++ standard: '${CMAKE_CXX_STANDARD}'.")

# A compiler accepting -std=c++23 does not guarantee its standard library has
# the ranges features used by podio and its generated headers.
include(CheckCXXSourceCompiles)
check_cxx_source_compiles([=[
  #include <algorithm>
  #include <ranges>
  #include <vector>

  int main() {
    [[maybe_unused]] std::from_range_t tag = std::from_range;
    auto values = std::ranges::to<std::vector<int>>(std::views::iota(0, 3));
    values.append_range(std::views::iota(3, 5));
    return !std::ranges::contains(values, 4);
  }
]=] PODIO_HAVE_CXX23_RANGES)
if(NOT PODIO_HAVE_CXX23_RANGES)
  message(FATAL_ERROR "podio requires a C++23 standard library with std::ranges::contains, std::ranges::to, std::from_range and container append_range (e.g. libstdc++ 15+ or libc++ 19+).")
endif()

# python setup (including python package discovery and install dir)
podio_python_setup()

#--- enable podio macros--------------------------------------------------------
include(cmake/podioMacros.cmake)

# optionally build with SIO -------------------------------------------------
if(ENABLE_SIO)
  find_package( SIO REQUIRED)
  # Targets from SIO only become available with v00-01 so we rig them here to be
  # able to use them
  if (SIO_VERSION VERSION_LESS 0.1)
    message(STATUS "Found SIO without Targets, creating them on the fly")
    add_library(SIO::sio SHARED IMPORTED)
    set_target_properties(SIO::sio PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES ${SIO_INCLUDE_DIRS}
      INTERFACE_LINK_LIBRARIES ${SIO_LIBRARIES}
      IMPORTED_LOCATION ${SIO_LIBRARIES})
  endif()

  if (SIO_FOUND)
    MESSAGE( STATUS "Found SIO library - will build SIO I/O support" )
    list(APPEND PODIO_IO_HANDLERS SIO)
  endif()
endif()

if(ENABLE_PARQUET AND NOT ENABLE_ARROW)
  message(FATAL_ERROR "ENABLE_PARQUET requires ENABLE_ARROW to be ON")
endif()

# optionally build with Arrow -----------------------------------------------
if(ENABLE_ARROW)
  set(lz4Alt_FIND_QUIETLY TRUE)
  set(zstdAlt_FIND_QUIETLY TRUE)
  find_package(Arrow REQUIRED)
  if(TARGET Arrow::arrow_shared)
    set(PODIO_ARROW_TARGET Arrow::arrow_shared)
  elseif(TARGET Arrow::arrow_static)
    set(PODIO_ARROW_TARGET Arrow::arrow_static)
  else()
    message(FATAL_ERROR "Found Arrow, but no usable Arrow CMake target was exported")
  endif()

  if(ENABLE_PARQUET)
    find_package(nlohmann_json 3.10 REQUIRED)
    find_package(Parquet REQUIRED)
    if(TARGET Parquet::parquet_shared)
      set(PODIO_PARQUET_TARGET Parquet::parquet_shared)
    else()
      set(PODIO_PARQUET_TARGET Parquet::parquet_static)
    endif()
    message(STATUS "Found Arrow and Parquet libraries - will build Arrow I/O support")
  else()
    message(STATUS "Found Arrow library - will build Arrow I/O support without Parquet")
  endif()

  list(APPEND PODIO_IO_HANDLERS ARROW)
endif()

#--- enable unit testing capabilities ------------------------------------------
include(CTest)
set(USE_EXTERNAL_CATCH2 AUTO CACHE STRING "Link against an external Catch2 v3 static library, otherwise build it locally")
set_property(CACHE USE_EXTERNAL_CATCH2 PROPERTY STRINGS AUTO ON OFF)

#--- target for Doxygen documentation ------------------------------------------
if(CREATE_DOC)
	include(cmake/podioDoxygen.cmake)
endif()
#--- add version files ---------------------------------------------------------
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/podioVersion.in.h
               ${CMAKE_CURRENT_SOURCE_DIR}/include/podio/podioVersion.h )
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/podio/podioVersion.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/podio )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/python/__version__.py.in
               ${CMAKE_CURRENT_SOURCE_DIR}/python/podio/__version__.py)
# Quality of life improvement to be able to use podio-dump --help
# without having to import ROOT
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/python/__version__.py.in
               ${CMAKE_CURRENT_SOURCE_DIR}/python/podio_version.py)


#--- add license files ---------------------------------------------------------
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
        DESTINATION ${CMAKE_INSTALL_DOCDIR})

#--- project specific subdirectories -------------------------------------------
add_subdirectory(src)

# Set the podio_PYTHON_DIR manually here because the macros in tests expect it
# If testing is not build this helps when building together with other packages
SET(podio_PYTHON_DIR ${PROJECT_SOURCE_DIR}/python CACHE PATH "Path to the podio python directory")

if(BUILD_TESTING)
  include(ExternalData)
  list(APPEND ExternalData_URL_TEMPLATES
    "https://key4hep.web.cern.ch:443/testFiles/podio/%(hash)"
  )
  include(cmake/podioTest.cmake)
  add_subdirectory(tests)
endif()

add_subdirectory(tools)
add_subdirectory(python)


if(BUILD_TESTING)
  # Make sure to fetch all data, after all legacy test cases have been added
  ExternalData_Add_Target(legacy_test_cases)
  message(STATUS "Test inputs will be stored in: ${ExternalData_OBJECT_STORES} if they are not already present")
endif()
#--- add CMake infrastructure --------------------------------------------------
include(cmake/podioCreateConfig.cmake)

#--- code format targets -------------------------------------------------------
include(cmake/pythonFormat.cmake)
