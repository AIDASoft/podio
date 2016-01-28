include_directories(
	${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/examples/datamodel
#        ${CMAKE_CURRENT_SOURCE_DIR}/utilities
)

find_package(GTest)
include_directories(${GTEST_INCLUDE_DIRS})

foreach( _conf ${CMAKE_CONFIGURATION_TYPES} )
  string(TOUPPER ${_conf} _conf )
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
  set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
endforeach()

file(GLOB executables RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

foreach( sourcefile ${executables} )
  string( REPLACE ".cpp" "" name ${sourcefile} )
  add_executable( ${name} ${sourcefile} )
  target_link_libraries( ${name} TestDataModelDict ${GTEST_LIBRARIES})
  install(TARGETS ${name} DESTINATION tests)
endforeach( sourcefile ${executables} )


#--- Adding tests --------------------------------------------------------------
add_test(NAME unittest COMMAND unittest)
