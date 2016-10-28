#--- CMake 3.3 to support C++11 features on all platforms -----------------------
cmake_minimum_required(VERSION 3.3)

#--- Project - only MAJOR.MINOR number is important for Podio API ---------------
project(podio VERSION 0.1.0)
set(podio_VERSION "${podio_VERSION_MAJOR}.${podio_VERSION_MINOR}")

#--- Define basic build settings -----------------------------------------------
# - Use GNU-style hierarchy for installing build products
include(GNUInstallDirs)

# - Define a default build type when using a single-mode tool like make/ninja
# If you're using a build tool that supports multiple modes (Visual Studio,
# Xcode), this setting has no effect.
# HSF recommend RelWithDebInfo (optimized with debugging symbols) as this is
# generally the mode used by system packaging (rpm, deb, spack, macports).
# However, it can be overriden by passing ``-DCMAKE_BUILD_TYPE=<type>`` when
# invoking CMake
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

# - Set compiler/linker flags etc
# - Define the C++ Standard to use (Simplest Possible)
# This will add any compiler flags needed to compile against the required standard
# without any vendor extensions
# NOTE: It *does not* guarantee that the compiler in use supports the complete
# standard. Nor does it inform clients of the project what standard was used.
# Both of these issues can be resolved using CMake's compile features, see
#
# - https://cmake.org/cmake/help/v3.3/manual/cmake-compile-features.7.html
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "-Wno-unused-variable -Wno-unused-parameter -pthread")
#add_definitions(-Wpadded)

# - Use .so extension for shared libs on macOS so ROOT can load them in the
#   interpreter
if(APPLE)
	set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif()

#--- Declare options -----------------------------------------------------------
option(podio_documentation "Whether or not to create doxygen doc target." ON)

#--- Declare ROOT dependency ---------------------------------------------------
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(ROOT REQUIRED COMPONENTS RIO Tree)
include_directories(${ROOT_INCLUDE_DIR})

#--temporary fix of inconsistency in ROOT CMake macros
set(ROOT_genreflex_cmd ${ROOT_genreflex_CMD})


#--- enable unit testing capabilities ------------------------------------------
include(CTest)

#--- enable CPack --------------------------------------------------------------
include(cmake/podioCPack.cmake)

#--- target for Doxygen documentation ------------------------------------------
if(podio_documentation)
	include(cmake/podioDoxygen.cmake)
endif()

#--- add version files ---------------------------------------------------------
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/podioVersion.h
               ${CMAKE_CURRENT_BINARY_DIR}/podioVersion.h )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/podioVersion.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/podio )

#--- add CMake infrastructure --------------------------------------------------
include(cmake/podioCreateConfig.cmake)

#--- add license files ---------------------------------------------------------
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
              ${CMAKE_CURRENT_SOURCE_DIR}/NOTICE
              DESTINATION ${CMAKE_INSTALL_DOCDIR})

#--- install templates ---------------------------------------------------------
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/templates
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/podio)

#--- project specific subdirectories -------------------------------------------
add_subdirectory(python)
add_subdirectory(src)
add_subdirectory(tests)


