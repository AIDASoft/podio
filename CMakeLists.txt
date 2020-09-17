# HSF recommends 3.3 to support C/C++ compile features for C/C++11 across all
# platforms
cmake_minimum_required(VERSION 3.12)

#--- Project name --------------------------------------------------------------
project(podio)

#--- Version -------------------------------------------------------------------
SET( ${PROJECT_NAME}_VERSION_MAJOR 0 )
SET( ${PROJECT_NAME}_VERSION_MINOR 12 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )

SET( ${PROJECT_NAME}_VERSION  "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}" )

#--- Define basic build settings -----------------------------------------------
# Provides install directory variables as defined for GNU software
include(GNUInstallDirs)

# Define a default build type can be overriden by passing
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

# Set up C++ Standard
# ``-DCMAKE_CXX_STANDARD=<standard>`` when invoking CMake
set(CMAKE_CXX_STANDARD 17 CACHE STRING "")

if(NOT CMAKE_CXX_STANDARD MATCHES "17")
  message(FATAL_ERROR "Unsupported C++ standard: ${CMAKE_CXX_STANDARD}")
endif()

# Prevent CMake falls back to the latest standard the compiler does support
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Disables the use of compiler-specific extensions, hence makes sure the code
# works for a wider range of compilers
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "Setting C++ standard: '${CMAKE_CXX_STANDARD}'.")

if (${APPLE})
    set(CPP_STANDARD_FLAGS "-std=c++${CMAKE_CXX_STANDARD}\ -stdlib=libc++")
    set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPP_STANDARD_FLAGS}")


# allow undefined symbols in shared library - needed for PythonEventStore/ROOTReader 
#
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    SET ( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-undefined,dynamic_lookup")
elseif ( ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" ))
    SET ( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--allow-shlib-undefined")
else()
    MESSAGE( WARNING "We do not test with the ${CMAKE_CXX_COMPILER_ID} compiler, use at your own discretion" )
endif()

#--- Declare options -----------------------------------------------------------
option(CREATE_DOC "Whether or not to create doxygen doc target." OFF)
option(ENABLE_SIO "Build the SIO block handlers and readers and writers." OFF)


option(USE_BOOST_FS "Use the boost filesystem library (in case your system does not yet have it)" OFF)

#--- Declare ROOT dependency ---------------------------------------------------
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(ROOT REQUIRED COMPONENTS RIO Tree)

# Check that root is compiled with a modern enough c++ standard
get_target_property(ROOT_COMPILE_FEATURES ROOT::Core INTERFACE_COMPILE_FEATURES)
if (NOT "cxx_std_17" IN_LIST ROOT_COMPILE_FEATURES)
  message(FATAL_ERROR "You are trying to build podio against a version of ROOT that has not been built with a sufficient c++ standard. podio requires c++17")
endif()
# Also verify that it has been built against the same python version as we are
# using (that is if ROOT gives us the necessary tools)
if (${ROOT_VERSION} VERSION_GREATER_EQUAL 6.19)
  set(Python_FIND_FRAMEWORK LAST)
  find_package(Python COMPONENTS Development)
  if (NOT ${Python_VERSION} EQUAL ${ROOT_PYTHON_VERSION})
    message(WARNING "Python version used for buildig ROOT is different than the current python version: " ${ROOT_PYTHON_VERSION} " vs " ${Python_VERSION})
  endif()
endif()

# ROOT only sets usage requirements from 6.14, so for
# earlier versions need to hack in INTERFACE_INCLUDE_DIRECTORIES
if(ROOT_VERSION VERSION_LESS 6.14)
  set_property(TARGET ROOT::Core APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${ROOT_INCLUDE_DIRS}")
endif()
list(APPEND PODIO_IO_HANDLERS ROOT)

#--- optionally build with SIO -------------------------------------------------
if(ENABLE_SIO)
  find_package( SIO REQUIRED)
  if (SIO_FOUND)
    MESSAGE( STATUS "Found SIO library - will build SIO I/O support" )
    list(APPEND PODIO_IO_HANDLERS SIO)
  endif()
endif()

#--- enable podio macros--------------------------------------------------------
include(cmake/podioMacros.cmake)

#--- enable unit testing capabilities ------------------------------------------
include(CTest)

#--- enable CPack --------------------------------------------------------------

option(ENABLE_CPACK "Whether or not to use cpack config" OFF)
if(ENABLE_CPACK)
  include(cmake/podioCPack.cmake)
endif()

#--- target for Doxygen documentation ------------------------------------------
if(CREATE_DOC)
	include(cmake/podioDoxygen.cmake)
endif()
#--- add version files ---------------------------------------------------------
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/podioVersion.h
               ${CMAKE_CURRENT_BINARY_DIR}/podioVersion.h )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/podioVersion.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/podio )

#--- add license files ---------------------------------------------------------
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
              ${CMAKE_CURRENT_SOURCE_DIR}/NOTICE
        DESTINATION ${CMAKE_INSTALL_DOCDIR})

#--- project specific subdirectories -------------------------------------------
add_subdirectory(python)
add_subdirectory(src)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

#--- add CMake infrastructure --------------------------------------------------
include(cmake/podioCreateConfig.cmake)


