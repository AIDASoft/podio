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
target_link_libraries(TestDataModel PUBLIC podio::podio)
target_include_directories(TestDataModel PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/
  )

add_library(TestDataModelDict SHARED)
target_link_libraries(TestDataModelDict PUBLIC TestDataModel podio::podio ROOT::Core)
target_include_directories(TestDataModelDict PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/
  ${ROOT_INCLUDE_DIRS}
 )

PODIO_GENERATE_DICTIONARY(TestDataModelDict ${headers} SELECTION src/selection.xml
  OPTIONS --library ${CMAKE_SHARED_LIBRARY_PREFIX}TestDataModelDict${CMAKE_SHARED_LIBRARY_SUFFIX}
  )
set_target_properties(TestDataModelDict-dictgen PROPERTIES EXCLUDE_FROM_ALL TRUE)
add_dependencies(TestDataModelDict TestDataModel)

#--- small utility helper function to allow for a more terse definition of tests below
function(CREATE_PODIO_TEST sourcefile needs_root)
  string( REPLACE ".cpp" "" name ${sourcefile} )
  add_executable( ${name} ${sourcefile} )
  add_test(NAME ${name} COMMAND ${name})
  if (needs_root)
    target_link_libraries(${name} TestDataModel TestDataModelDict podio::podioRootIO)
    set_property(TEST ${name} PROPERTY ENVIRONMENT
      LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH}
      ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/tests/datamodel:${ROOT_INCLUDE_PATH})
  else()
    target_link_libraries(${name} TestDataModel)
    set_property(TEST ${name} PROPERTY ENVIRONMENT
      LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH})
  endif()
endfunction()

set(root_dependent_tests write.cpp read.cpp read-multiple.cpp relation_range.cpp)
foreach( sourcefile ${root_dependent_tests} )
  CREATE_PODIO_TEST(${sourcefile} 1)
endforeach()

set(root_independent_tests ostream_operator.cpp unittest.cpp write_sio.cpp read_sio.cpp)
if (NOT SIO_FOUND)
  list(FILTER ${root_independent_tests} EXCLUDE REGEX "sio")
endif()
foreach( sourcefile ${root_independent_tests} )
  CREATE_PODIO_TEST(${sourcefile} 0)
endforeach()

#--- set some dependencies between the different tests to ensure input generating ones are run first
set_property(TEST read PROPERTY DEPENDS write)
set_property(TEST read-multiple PROPERTY DEPENDS write)
if (SIO_FOUND)
  set_property(TEST read_sio PROPERTY DEPENDS write_sio)
endif()

add_test( NAME pyunittest COMMAND python -m unittest discover -s ${CMAKE_SOURCE_DIR}/python)
set_property(TEST pyunittest
             PROPERTY ENVIRONMENT
                      LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH}
                      PYTHONPATH=${CMAKE_SOURCE_DIR}/python:$ENV{PYTHONPATH}
                      ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/tests/datamodel:${ROOT_INCLUDE_PATH})
set_property(TEST pyunittest PROPERTY DEPENDS write)
