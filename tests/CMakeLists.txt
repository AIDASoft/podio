include_directories(
	${CMAKE_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/datamodel
)

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

REFLEX_GENERATE_DICTIONARY(TestDataModel ${headers} SELECTION src/selection.xml)
add_library(TestDataModelDict SHARED TestDataModel.cxx)
add_dependencies(TestDataModelDict TestDataModel-dictgen)
target_link_libraries(TestDataModelDict TestDataModel podio )

file(GLOB executables RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

foreach( sourcefile ${executables} )
  string( REPLACE ".cpp" "" name ${sourcefile} )
  add_executable( ${name} ${sourcefile} )
  target_link_libraries( ${name} TestDataModelDict )
endforeach( sourcefile ${executables} )


#--- Adding tests --------------------------------------------------------------
set(__dynamic_loader_envvar LD_LIBRARY_PATH)
if(APPLE)
  set(__dynamic_loader_envvar DYLD_FALLBACK_LIBRARY_PATH)
endif()

add_test(NAME write COMMAND write)
set_property(TEST write PROPERTY ENVIRONMENT ${__dynamic_loader_envvar}=${PROJECT_BINARY_DIR}/src:${PROJECT_BINARY_DIR}/tests:$ENV{${__dynamic_loader_envvar}})

add_test(NAME read COMMAND read)
set_property(TEST read PROPERTY ENVIRONMENT ${__dynamic_loader_envvar}=${PROJECT_BINARY_DIR}/src:${PROJECT_BINARY_DIR}/tests:$ENV{${__dynamic_loader_envvar}})
set_property(TEST read PROPERTY DEPENDS write)

add_test( NAME pyunittest COMMAND python -m unittest discover -s ${PROJECT_SOURCE_DIR}/python)
set_property(TEST pyunittest PROPERTY ENVIRONMENT
  ${__dynamic_loader_envvar}=${PROJECT_BINARY_DIR}/src:${PROJECT_BINARY_DIR}/tests:$ENV{${__dynamic_loader_envvar}}
  PYTHONPATH=${PROJECT_SOURCE_DIR}/python:$ENV{PYTHONPATH}
  )
set_property(TEST pyunittest PROPERTY DEPENDS write)

add_test(NAME unittest COMMAND unittest)
