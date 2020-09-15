foreach( _conf ${CMAKE_CONFIGURATION_TYPES} )
  string(TOUPPER ${_conf} _conf )
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
  set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
endforeach()

PODIO_GENERATE_DATAMODEL(datamodel datalayout.yaml headers sources)

if (SIO_HANDLERS)
  # Filter out the sio block handlers to avoid polluting the TestDataModel lib
  # with sio related things
  SET(sio_headers ${headers})
  SET(sio_sources ${sources})

  LIST(FILTER headers EXCLUDE REGEX .*SIOBlock.h)
  LIST(FILTER sio_headers INCLUDE REGEX .*SIOBlock.h)

  LIST(FILTER sources EXCLUDE REGEX .*SIOBlock.cc)
  LIST(FILTER sio_sources INCLUDE REGEX .*SIOBlock.cc)
endif()

add_library(TestDataModel SHARED ${sources} ${headers} )
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
 )

PODIO_GENERATE_DICTIONARY(TestDataModelDict ${headers} SELECTION src/selection.xml
  OPTIONS --library ${CMAKE_SHARED_LIBRARY_PREFIX}TestDataModelDict${CMAKE_SHARED_LIBRARY_SUFFIX}
  )
set_target_properties(TestDataModelDict-dictgen PROPERTIES EXCLUDE_FROM_ALL TRUE)
add_dependencies(TestDataModelDict TestDataModel)

if (SIO_HANDLERS)
  add_library(TestDataModelSioBlocks SHARED ${sio_sources} ${sio_headers})
  target_link_libraries(TestDataModelSioBlocks PUBLIC podio::podio)
  target_include_directories(TestDataModelSioBlocks PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${SIO_INCLUDE_DIRS}
  )
  # add_dependencies(TestDataModelSioBlocks TestDataModel)
endif()

#--- small utility helper function to allow for a more terse definition of tests below
function(CREATE_PODIO_TEST sourcefile additional_libs)
  string( REPLACE ".cpp" "" name ${sourcefile} )
  add_executable( ${name} ${sourcefile} )
  add_test(NAME ${name} COMMAND ${name})

  target_link_libraries(${name} TestDataModel ${additional_libs})
  set_property(TEST ${name} PROPERTY ENVIRONMENT
      LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH}
      ${additional_env}
    )
endfunction()

set(root_dependent_tests write.cpp read.cpp read-multiple.cpp relation_range.cpp)
set(root_libs TestDataModelDict podio::podioRootIO)
set(root_env ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/tests/datamodel:${ROOT_INCLUDE_PATH})
foreach( sourcefile ${root_dependent_tests} )
  CREATE_PODIO_TEST(${sourcefile} "${root_libs}" ${root_env})
endforeach()


set(independent_tests ostream_operator.cpp unittest.cpp)
foreach( sourcefile ${independent_tests} )
  CREATE_PODIO_TEST(${sourcefile} "" "")
endforeach()


if (SIO_HANDLERS)
  set(sio_dependent_tests write_sio.cpp read_sio.cpp )
  set(sio_libs podio::podioSioIO)
  foreach( sourcefile ${sio_dependent_tests} )
    CREATE_PODIO_TEST(${sourcefile} "${sio_libs}" "")
  endforeach()
endif()

#--- set some dependencies between the different tests to ensure input generating ones are run first
set_property(TEST read PROPERTY DEPENDS write)
set_property(TEST read-multiple PROPERTY DEPENDS write)
if (SIO_HANDLERS)
  set_property(TEST read_sio PROPERTY DEPENDS write_sio)
endif()

add_test( NAME pyunittest COMMAND python -m unittest discover -s ${CMAKE_SOURCE_DIR}/python)
set_property(TEST pyunittest
             PROPERTY ENVIRONMENT
                      LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH}
                      PYTHONPATH=${CMAKE_SOURCE_DIR}/python:$ENV{PYTHONPATH}
                      ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/tests/datamodel:${ROOT_INCLUDE_PATH})
set_property(TEST pyunittest PROPERTY DEPENDS write)
