include_directories(
	${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/datamodel
#        ${CMAKE_CURRENT_SOURCE_DIR}/utilities
)

find_package(GTest)

foreach( _conf ${CMAKE_CONFIGURATION_TYPES} )
  string(TOUPPER ${_conf} _conf )
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
  set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
endforeach()

file(GLOB sources datamodel/*.cc)
file(GLOB headers datamodel/*.h)
file(GLOB sources_utils utilities/*.cc)
file(GLOB headers_utils utilities/*.h)

list(REMOVE_ITEM headers ${CMAKE_CURRENT_SOURCE_DIR}/datamodel/LinkDef.h )
REFLEX_GENERATE_DICTIONARY(DataModelEx ${headers} SELECTION datamodel/selection.xml)

add_library(DataModelExample SHARED ${sources} ${sources_utils} ${headers} ${headers_utils} DataModelEx.cxx)
target_link_libraries(DataModelExample podio )

file(GLOB executables RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

foreach( sourcefile ${executables} )
  string( REPLACE ".cpp" "" name ${sourcefile} )
  add_executable( ${name} ${sourcefile} )
  target_link_libraries( ${name} DataModelExample ${GTEST_LIBRARIES})
  install(TARGETS ${name} DESTINATION examples)
endforeach( sourcefile ${executables} )

#--- Adding tests --------------------------------------------------------------
add_test(NAME write COMMAND write)
add_test(NAME read COMMAND read DEPENDS write)
add_test(NAME unittest COMMAND unittest)
