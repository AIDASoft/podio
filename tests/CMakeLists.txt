foreach( _conf ${CMAKE_CONFIGURATION_TYPES} )
  string(TOUPPER ${_conf} _conf )
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
  set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
endforeach()

PODIO_GENERATE_DATAMODEL(datamodel datalayout.yaml headers sources
  IO_BACKEND_HANDLERS ${PODIO_IO_HANDLERS})

# Use the cmake building blocks to add the different parts (conditionally)
PODIO_ADD_DATAMODEL_CORE_LIB(TestDataModel "${headers}" "${sources}")
PODIO_ADD_ROOT_IO_DICT(TestDataModelDict TestDataModel "${headers}" src/selection.xml)
PODIO_ADD_SIO_IO_BLOCKS(TestDataModel "${headers}" "${sources}")

#--- small utility helper function to allow for a more terse definition of tests below
function(CREATE_PODIO_TEST sourcefile additional_libs)
  string( REPLACE ".cpp" "" name ${sourcefile} )
  add_executable( ${name} ${sourcefile} )
  add_test(NAME ${name} COMMAND ${name})

  target_link_libraries(${name} TestDataModel ${additional_libs})
  set_property(TEST ${name} PROPERTY ENVIRONMENT
      LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH}
    )
endfunction()

set(root_dependent_tests write.cpp read.cpp read-multiple.cpp relation_range.cpp read_and_write.cpp write_timed.cpp read_timed.cpp)
set(root_libs TestDataModelDict podio::podioRootIO)
foreach( sourcefile ${root_dependent_tests} )
  CREATE_PODIO_TEST(${sourcefile} "${root_libs}")
endforeach()


set(independent_tests ostream_operator.cpp unittest.cpp write_ascii.cpp)
foreach( sourcefile ${independent_tests} )
  CREATE_PODIO_TEST(${sourcefile} "")
endforeach()


if (TARGET TestDataModelSioBlocks)
  set(sio_dependent_tests write_sio.cpp read_sio.cpp read_and_write_sio.cpp write_timed_sio.cpp read_timed_sio.cpp)
  set(sio_libs podio::podioSioIO)
  foreach( sourcefile ${sio_dependent_tests} )
    CREATE_PODIO_TEST(${sourcefile} "${sio_libs}")
  endforeach()

  # These need to be linked against TTree explicitly, since it is not done
  # through another library and the TimedReader/Writer decorators are
  # header-only wrappers
  target_link_libraries(write_timed_sio ROOT::Tree)
  target_link_libraries(read_timed_sio ROOT::Tree)
endif()


#--- set some dependencies between the different tests to ensure input generating ones are run first
set_property(TEST read PROPERTY DEPENDS write)
set_property(TEST read-multiple PROPERTY DEPENDS write)
set_property(TEST read_timed PROPERTY DEPENDS write_timed)

add_executable(check_benchmark_outputs check_benchmark_outputs.cpp)
target_link_libraries(check_benchmark_outputs ROOT::Tree)

add_test(NAME check_benchmark_outputs COMMAND check_benchmark_outputs write_benchmark_root.root read_benchmark_root.root)
set_property(TEST check_benchmark_outputs PROPERTY DEPENDS read_timed write_timed)

if (TARGET read_sio)
  set_property(TEST read_sio PROPERTY DEPENDS write_sio)
  set_property(TEST read_timed_sio PROPERTY DEPENDS write_timed_sio)

  add_test(NAME check_benchmark_outputs_sio COMMAND check_benchmark_outputs write_benchmark_sio.root read_benchmark_sio.root)
  set_property(TEST check_benchmark_outputs_sio PROPERTY DEPENDS read_timed_sio write_timed_sio)
endif()

add_test( NAME pyunittest COMMAND python -m unittest discover -s ${CMAKE_SOURCE_DIR}/python)
set_property(TEST pyunittest
             PROPERTY ENVIRONMENT
                      LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH}
                      PYTHONPATH=${CMAKE_SOURCE_DIR}/python:$ENV{PYTHONPATH}
                      ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/tests/datamodel:${ROOT_INCLUDE_PATH})
set_property(TEST pyunittest PROPERTY DEPENDS write)
