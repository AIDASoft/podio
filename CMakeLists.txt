cmake_minimum_required(VERSION 2.8)
project(albers)

# Declare ROOT dependency
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS RIO Tree)
include(${ROOT_USE_FILE})

add_definitions(-Wno-unused-variable -Wno-unused-parameter)
#add_definitions(-Wpadded)

#--temporary fix of inconsistency in ROOT CMake macros
set(ROOT_genreflex_cmd ${ROOT_genreflex_CMD})

add_subdirectory(src)
add_subdirectory(examples)

#--- target for Doxygen documentation-------------------

find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/documentation/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc
  ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)
