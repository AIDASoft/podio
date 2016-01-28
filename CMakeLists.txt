cmake_minimum_required(VERSION 2.8)

project(podio)
set( podio_VERSION_MAJOR 0 )
set( podio_VERSION_MINOR 1 )
set( podio_VERSION_PATCH 0 )
set( podio_VERSION "${podio_VERSION_MAJOR}.${podio_VERSION_MINOR}" )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


#--- Declare ROOT dependency ---------------------------------------------------
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(ROOT REQUIRED COMPONENTS RIO Tree)

add_definitions(-Wno-unused-variable -Wno-unused-parameter -pthread)
#add_definitions(-Wpadded)
#--temporary fix of inconsistency in ROOT CMake macros
# set(ROOT_genreflex_cmd ${ROOT_genreflex_CMD})
set(ROOT_genreflex_cmd ${ROOT_GENERATE_DICTIONARY})

add_definitions(-Wno-unused-variable -Wno-unused-parameter)

#--- enable unit testing capabilities ------------------------------------------
include(CTest)

#--- enable CPack --------------------------------------------------------------
include(cmake/podioCPack.cmake)

#--- target for Doxygen documentation ------------------------------------------
include(cmake/podioDoxygen.cmake)

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

#--- project specific subdirectories -------------------------------------------
add_subdirectory(python)
add_subdirectory(src)
add_subdirectory(tests)
