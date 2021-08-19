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

CREATE_PODIO_TEST(ostream_operator.cpp "")
CREATE_PODIO_TEST(write_ascii.cpp "")

if(USE_EXTERNAL_CATCH2)
  find_package(Catch2 REQUIRED)
else()
  message(STATUS "Fetching local copy of Catch2 library for unit-tests...")
  # Build Catch2 with the default flags, to avoid generating warnings when we
  # build it
  set(CXX_FLAGS_CMAKE_USED ${CMAKE_CXX_FLAGS})
  set(CMAKE_CXX_FLAGS ${CXX_FLAGS_CMAKE_DEFAULTS})
  Include(FetchContent)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        037ddbc75cc5e58b93cf5a010a94b32333ad824d
    )
  FetchContent_MakeAvailable(Catch2)
  set(CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras ${CMAKE_MODULE_PATH})


  # Hack around the fact, that the include directories are not declared as
  # SYSTEM for the targets defined this way. Otherwise warnings can still occur
  # in Catch2 code when templates are evaluated (which happens quite a bit)
  get_target_property(CATCH2_IF_INC_DIRS Catch2 INTERFACE_INCLUDE_DIRECTORIES)
  set_target_properties(Catch2 PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${CATCH2_IF_INC_DIRS}")

  # Reset the flags
  set(CMAKE_CXX_FLAGS ${CXX_FLAGS_CMAKE_USED})
endif()

CREATE_PODIO_TEST(unittest.cpp "Catch2::Catch2;Catch2::Catch2WithMain")
include(Catch)
catch_discover_tests(unittest WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})


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
set_property(TEST read_and_write PROPERTY DEPENDS write)
set_property(TEST read_timed PROPERTY DEPENDS write_timed)

add_executable(check_benchmark_outputs check_benchmark_outputs.cpp)
target_link_libraries(check_benchmark_outputs ROOT::Tree)

add_test(NAME check_benchmark_outputs COMMAND check_benchmark_outputs write_benchmark_root.root read_benchmark_root.root)
set_property(TEST check_benchmark_outputs PROPERTY DEPENDS read_timed write_timed)

if (TARGET read_sio)
  set_property(TEST read_sio PROPERTY DEPENDS write_sio)
  set_property(TEST read_and_write_sio PROPERTY DEPENDS write_sio)
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
