cmake_minimum_required(VERSION 2.8)

project(podio)
set( podio_VERSION_MAJOR 0 )
set( podio_VERSION_MINOR 9 )
set( podio_VERSION_PATCH 0 )
set( podio_VERSION "${podio_VERSION_MAJOR}.${podio_VERSION_MINOR}" )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

if(APPLE)
	set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif(APPLE)

#--- Declare options -----------------------------------------------------------
option(CREATE_DOC "Whether or not to create doxygen doc target.")

#--- Declare ROOT dependency ---------------------------------------------------
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(ROOT 6.08.06 REQUIRED COMPONENTS RIO Tree)
include_directories(${ROOT_INCLUDE_DIR})
include(${ROOT_USE_FILE})

add_definitions(-Wno-unused-variable -Wno-unused-parameter -pthread)
#add_definitions(-Wpadded)

add_definitions(-Wno-unused-variable -Wno-unused-parameter)

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
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/podio )

#--- add CMake infrastructure --------------------------------------------------
include(cmake/podioCreateConfig.cmake)

#--- add license files ---------------------------------------------------------
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
              ${CMAKE_CURRENT_SOURCE_DIR}/NOTICE
        DESTINATION ${CMAKE_INSTALL_PREFIX})

#--- install templates ---------------------------------------------------------
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/templates
		DESTINATION ${CMAKE_INSTALL_PREFIX})

#--- project specific subdirectories -------------------------------------------
add_subdirectory(python)
add_subdirectory(src)
add_subdirectory(tests)
