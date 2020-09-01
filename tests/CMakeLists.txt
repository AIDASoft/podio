foreach( _conf ${CMAKE_CONFIGURATION_TYPES} )
  string(TOUPPER ${_conf} _conf )
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
  set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
endforeach()

PODIO_GENERATE_DATAMODEL(datamodel datalayout.yaml headers sources)

file(GLOB sources_utils utilities/*.cc)
file(GLOB headers_utils utilities/*.h)

add_library(TestDataModel SHARED ${sources} ${sources_utils} ${headers} ${headers_utils})
target_link_libraries(TestDataModel podio::podio ROOT::Core)
target_include_directories(TestDataModel PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/
  )
PODIO_GENERATE_DICTIONARY(TestDataModel ${headers} SELECTION src/selection.xml
  OPTIONS --library ${CMAKE_SHARED_LIBRARY_PREFIX}TestDataModel${CMAKE_SHARED_LIBRARY_SUFFIX}
  )
set_target_properties(TestDataModel-dictgen PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_sources(TestDataModel PRIVATE TestDataModel.cxx)

file(GLOB executables RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

foreach( sourcefile ${executables} )
  string( REPLACE ".cpp" "" name ${sourcefile} )
  add_executable( ${name} ${sourcefile} )
  target_link_libraries( ${name} TestDataModel podio::podioRootIO)
endforeach( sourcefile ${executables} )
#--- Adding tests --------------------------------------------------------------

add_test(NAME write COMMAND write)
# The following directories need to be added to the LD_LIBRARY_PATH:
# - CMAKE_CURRENT_BINARY_DIR: contains the root dictionary and pcm files for TestDataModel
# - CMAKE_BINARY_DIR/src: contains the root dictionary and pcm files for podio
set_property(TEST write PROPERTY ENVIRONMENT LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH})

add_test(NAME read COMMAND read)
set_property(TEST read PROPERTY ENVIRONMENT
             LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH}
             ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/tests/datamodel:${ROOT_INCLUDE_PATH})
set_property(TEST read PROPERTY DEPENDS write)

add_test(NAME read-multiple COMMAND read-multiple)
set_property(TEST read-multiple PROPERTY ENVIRONMENT
             LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH}
             ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/tests/datamodel:${ROOT_INCLUDE_PATH})
set_property(TEST read-multiple PROPERTY DEPENDS write)

add_test(NAME relation_range COMMAND relation_range)
set_property(TEST relation_range PROPERTY ENVIRONMENT
  LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH}
  ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/tests/datamodel:${ROOT_INCLUDE_PATH})

add_test( NAME pyunittest COMMAND python -m unittest discover -s ${CMAKE_SOURCE_DIR}/python)
set_property(TEST pyunittest
             PROPERTY ENVIRONMENT
                      LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH}
                      PYTHONPATH=${CMAKE_SOURCE_DIR}/python:$ENV{PYTHONPATH}
                      ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/tests/datamodel:${ROOT_INCLUDE_PATH})
set_property(TEST pyunittest PROPERTY DEPENDS write)

add_test(NAME ostream_operator COMMAND ostream_operator)
set_property(TEST ostream_operator PROPERTY ENVIRONMENT
  LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH}
  ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/tests/datamodel:${ROOT_INCLUDE_PATH})

add_test(NAME unittest COMMAND unittest)

add_test(NAME write_sio COMMAND write_sio)
set_property(TEST write_sio PROPERTY ENVIRONMENT
  LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH})

add_test(NAME read_sio COMMAND read_sio)
set_property(TEST read_sio PROPERTY ENVIRONMENT
  LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH})
