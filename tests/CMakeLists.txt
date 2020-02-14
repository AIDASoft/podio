foreach( _conf ${CMAKE_CONFIGURATION_TYPES} )
  string(TOUPPER ${_conf} _conf )
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
  set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
endforeach()

file(GLOB sources src/*.cc)
file(GLOB headers datamodel/*.h)
file(GLOB sources_utils utilities/*.cc)
file(GLOB headers_utils utilities/*.h)

add_library(TestDataModel SHARED ${sources} ${sources_utils} ${headers} ${headers_utils})
target_link_libraries(TestDataModel podio )
target_include_directories(TestDataModel PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/datamodel
)

PODIO_GENERATE_DICTIONARY(TestDataModel ${headers} SELECTION src/selection.xml USES TestDataModel)
add_library(TestDataModelDict SHARED TestDataModel.cxx)
add_dependencies(TestDataModelDict TestDataModel-dictgen)
target_link_libraries(TestDataModelDict TestDataModel podio )

file(GLOB executables RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

foreach( sourcefile ${executables} )
  string( REPLACE ".cpp" "" name ${sourcefile} )
  add_executable( ${name} ${sourcefile} )
  target_link_libraries( ${name} TestDataModelDict podio)
endforeach( sourcefile ${executables} )

#--- Adding tests --------------------------------------------------------------
# uncomment this to actually run the EDM generation in the tests
add_test(NAME generate-edm
	 COMMAND python ${CMAKE_SOURCE_DIR}/python/podio_class_generator.py tests/datalayout.yaml tests datamodel --dryrun
	 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_test(NAME write COMMAND write)
# The following directories need to be added to the LD_LIBRARY_PATH:
# - CMAKE_CURRENT_BINARY_DIR: contains the root dictionary and pcm files for TestDataModelDict
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

add_test( NAME pyunittest COMMAND python -m unittest discover -s ${CMAKE_SOURCE_DIR}/python)
set_property(TEST pyunittest
             PROPERTY ENVIRONMENT
                      LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH}
                      PYTHONPATH=${CMAKE_SOURCE_DIR}/python:$ENV{PYTHONPATH}
                      ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/tests/datamodel:${ROOT_INCLUDE_PATH})
set_property(TEST pyunittest PROPERTY DEPENDS write)

add_test(NAME unittest COMMAND unittest)

# Fail tests if there is any error in the output
set_tests_properties(generate-edm write read read-multiple pyunittest unittest
    PROPERTIES FAIL_REGULAR_EXPRESSION "[^a-z]Error;ERROR;Failed"
)

