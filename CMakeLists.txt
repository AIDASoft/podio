# HSF recommends 3.3 to support C/C++ compile features for C/C++11 across all
# platforms
cmake_minimum_required(VERSION 3.3)

#--- Project name --------------------------------------------------------------
project(podio)

#--- Version -------------------------------------------------------------------
set( podio_VERSION_MAJOR 0 )
set( podio_VERSION_MINOR 9 )
set( podio_VERSION_PATCH 0 )
set( podio_VERSION "${podio_VERSION_MAJOR}.${podio_VERSION_MINOR}" )

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
set(CMAKE_CXX_STANDARD 14 CACHE STRING "")

if(NOT CMAKE_CXX_STANDARD MATCHES "14|17")
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


#--- Declare options -----------------------------------------------------------
option(CREATE_DOC "Whether or not to create doxygen doc target.")

#--- Declare ROOT dependency ---------------------------------------------------
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(ROOT REQUIRED COMPONENTS RIO Tree)
include_directories(${ROOT_INCLUDE_DIR})
include(${ROOT_USE_FILE})

add_definitions(-Wno-unused-variable -Wno-unused-parameter -pthread)

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
		DESTINATION ${CMAKE_INSTALL_PREFIX})

#--- project specific subdirectories -------------------------------------------
add_subdirectory(python)
add_subdirectory(src)
add_subdirectory(tests)
