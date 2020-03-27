# HSF recommends 3.3 to support C/C++ compile features for C/C++11 across all
# platforms
cmake_minimum_required(VERSION 3.3)

#--- Project name --------------------------------------------------------------
project(podio VERSION 0.9.1)

#--- Version -------------------------------------------------------------------

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


if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    SET ( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-undefined,error")
elseif ( ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" ))
    SET ( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
else()
    MESSAGE( WARNING "We do not test with the ${CMAKE_CXX_COMPILER_ID} compiler, use at your own discretion" )
endif()


#--- Declare options -----------------------------------------------------------
option(CREATE_DOC "Whether or not to create doxygen doc target." OFF)

#--- Declare ROOT dependency ---------------------------------------------------
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(ROOT REQUIRED COMPONENTS RIO Tree)
# ROOT only sets usage requirements from 6.14, so for
# earlier versions need to hack in INTERFACE_INCLUDE_DIRECTORIES
if(ROOT_VERSION VERSION_LESS 6.14)
  set_property(TARGET ROOT::Core APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${ROOT_INCLUDE_DIRS}")
endif()

#--- enable podio macros--------------------------------------------------------
include(cmake/podioMacros.cmake)

#--- enable unit testing capabilities ------------------------------------------
include(CTest)

#--- enable CPack --------------------------------------------------------------
include(cmake/podioCPack.cmake)

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


