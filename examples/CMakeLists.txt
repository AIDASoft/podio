include_directories(
	${CMAKE_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/datamodel
#        ${CMAKE_CURRENT_SOURCE_DIR}/utilities
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
install(TARGETS TestDataModel DESTINATION examples)

REFLEX_GENERATE_DICTIONARY(TestDataModel ${headers} SELECTION datamodel/selection.xml)
add_library(TestDataModelDict SHARED TestDataModel.cxx)
target_link_libraries(TestDataModelDict TestDataModel podio )
install(TARGETS TestDataModelDict DESTINATION examples)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/TestDataModelDict.rootmap DESTINATION examples)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/TestDataModel_rdict.pcm DESTINATION examples)


file(GLOB executables RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

foreach( sourcefile ${executables} )
  string( REPLACE ".cpp" "" name ${sourcefile} )
  add_executable( ${name} ${sourcefile} )
  target_link_libraries( ${name} TestDataModelDict ${GTEST_LIBRARIES})
  install(TARGETS ${name} DESTINATION examples)
endforeach( sourcefile ${executables} )


#--- Adding examples --------------------------------------------------------------
add_test(NAME write COMMAND write)
set_property(TEST write PROPERTY ENVIRONMENT LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/examples:$ENV{LD_LIBRARY_PATH})

add_test(NAME read COMMAND read)
set_property(TEST read PROPERTY ENVIRONMENT LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/examples:$ENV{LD_LIBRARY_PATH})
set_property(TEST read PROPERTY DEPENDS write)

add_test( NAME pyunittest COMMAND python -m unittest discover -s ${CMAKE_INSTALL_PREFIX}/python)
set_property(TEST pyunittest PROPERTY ENVIRONMENT LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/examples:$ENV{LD_LIBRARY_PATH} PYTHONPATH=${CMAKE_INSTALL_PREFIX}/python:$ENV{PYTHONPATH})
set_property(TEST pyunittest PROPERTY DEPENDS write)
