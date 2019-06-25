find_package(HDF5 COMPONENTS CXX REQUIRED)


#target_link_libraries(dummy PUBLIC ${HDF5_LIBRARIES} ${HDF5_CXX_LIBRARIES})



include_directories(
	${CMAKE_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/datamodel ${HDF5_INCLUDE_DIRS})




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
target_link_libraries(TestDataModel ${HDF5_LIBRARIES} ${HDF5_CXX_LIBRARIES})
install(TARGETS TestDataModel DESTINATION tests)

REFLEX_GENERATE_DICTIONARY(TestDataModel ${headers} SELECTION src/selection.xml)
add_library(TestDataModelDict SHARED TestDataModel.cxx)
add_dependencies(TestDataModelDict TestDataModel-dictgen)
target_link_libraries(TestDataModelDict TestDataModel podio )
install(TARGETS TestDataModelDict DESTINATION tests)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/TestDataModelDict.rootmap DESTINATION tests)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/TestDataModel_rdict.pcm DESTINATION tests)


file(GLOB executables RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

foreach( sourcefile ${executables} )
  string( REPLACE ".cpp" "" name ${sourcefile} )
  add_executable( ${name} ${sourcefile} )
  target_link_libraries( ${name} TestDataModelDict )
  install(TARGETS ${name} DESTINATION tests)
endforeach( sourcefile ${executables} )


#--- Adding tests --------------------------------------------------------------
# uncomment this to actually run the EDM generation in the tests
#add_test(NAME generate-edm COMMAND python $ENV{PODIO}/python/podio_class_generator.py tests/datalayout.yaml tests datamodel --clangformat
add_test(NAME generate-edm COMMAND python $ENV{PODIO}/python/podio_class_generator.py tests/datalayout.yaml tests datamodel --dryrun --clangformat
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
set_property(TEST generate-edm PROPERTY ENVIRONMENT LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/tests:$ENV{LD_LIBRARY_PATH})


add_test(NAME write COMMAND write)
set_property(TEST write PROPERTY ENVIRONMENT LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/tests:$ENV{LD_LIBRARY_PATH})

add_test(NAME read COMMAND read)
set_property(TEST read PROPERTY ENVIRONMENT LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/tests:$ENV{LD_LIBRARY_PATH})
set_property(TEST read PROPERTY DEPENDS write)

add_test(NAME read-multiple COMMAND read-multiple)
set_property(TEST read-multiple PROPERTY ENVIRONMENT LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/tests:$ENV{LD_LIBRARY_PATH})
set_property(TEST read-multiple PROPERTY DEPENDS write)

add_test( NAME pyunittest COMMAND python -m unittest discover -s ${CMAKE_INSTALL_PREFIX}/python)
set_property(TEST pyunittest
             PROPERTY ENVIRONMENT
                      LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/tests:$ENV{LD_LIBRARY_PATH}
                      PYTHONPATH=${CMAKE_INSTALL_PREFIX}/python:$ENV{PYTHONPATH}
                      ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/tests/datamodel)
set_property(TEST pyunittest PROPERTY DEPENDS write)

add_test(NAME unittest COMMAND unittest)



