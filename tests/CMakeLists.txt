include_directories(
	${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/datamodel
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

file(GLOB sources src/*.cc)
file(GLOB headers datamodel/*.h)
file(GLOB sources_utils utilities/*.cc)
file(GLOB headers_utils utilities/*.h)

add_library(TestDataModel SHARED ${sources} ${sources_utils} ${headers} ${headers_utils})
target_link_libraries(TestDataModel podio )
install(TARGETS TestDataModel DESTINATION tests)

REFLEX_GENERATE_DICTIONARY(TestDataModel ${headers} SELECTION datamodel/selection.xml)
add_library(TestDataModelDict SHARED TestDataModel.cxx)
target_link_libraries(TestDataModelDict TestDataModel podio )
install(TARGETS TestDataModelDict DESTINATION tests)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/TestDataModelDict.rootmap DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/TestDataModel_rdict.pcm DESTINATION lib)


file(GLOB executables RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

foreach( sourcefile ${executables} )
  string( REPLACE ".cpp" "" name ${sourcefile} )
  add_executable( ${name} ${sourcefile} )
  target_link_libraries( ${name} TestDataModelDict ${GTEST_LIBRARIES})
  install(TARGETS ${name} DESTINATION tests)
endforeach( sourcefile ${executables} )


#--- Adding tests --------------------------------------------------------------
add_test(NAME write COMMAND write)
set_property(TEST write PROPERTY ENVIRONMENT LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/tests:$ENV{LD_LIBRARY_PATH})

add_test(NAME read COMMAND read DEPENDS write)
set_property(TEST read PROPERTY ENVIRONMENT LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/tests:$ENV{LD_LIBRARY_PATH})

add_test(NAME unittest COMMAND unittest)

add_test(NAME read_python COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/read.py)
set_property(TEST read_python PROPERTY ENVIRONMENT PYTHONPATH=${CMAKE_INSTALL_PREFIX}/python:${CMAKE_INSTALL_PREFIX}/tests:$ENV{PYTHONPATH}
                                                   LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/tests:$ENV{LD_LIBRARY_PATH})
