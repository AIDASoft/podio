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
find_package(nlohmann_json 3.10)
if (nlohmann_json_FOUND)
  message(STATUS "Found compatible version of JSON library, will add JSON support to test datamodel")
  target_compile_definitions(TestDataModel PUBLIC PODIO_JSON_OUTPUT)
  target_link_libraries(TestDataModel PUBLIC nlohmann_json::nlohmann_json)
endif()

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
      # Clear the ROOT_INCLUDE_PATH for the tests, to avoid potential conflicts
      # with existing headers from other installations
      ROOT_INCLUDE_PATH=
    )
endfunction()

set(root_dependent_tests
  write.cpp
  read.cpp
  read-multiple.cpp
  relation_range.cpp
  read_and_write.cpp
  read_and_write_associated.cpp
  write_timed.cpp
  read_timed.cpp
  read_frame.cpp
  write_frame_root.cpp)
set(root_libs TestDataModelDict podio::podioRootIO)
foreach( sourcefile ${root_dependent_tests} )
  CREATE_PODIO_TEST(${sourcefile} "${root_libs}")
endforeach()

# Avoid fetching these everytime cmake is run by caching the directory the first
# time the inputs are fetched or if the expected file does not exist in the
# expected directory
if (NOT DEFINED CACHE{PODIO_TEST_INPUT_DATA_DIR} OR NOT EXISTS ${PODIO_TEST_INPUT_DATA_DIR}/example.root)
  message("Getting test input files")
  execute_process(
    COMMAND bash ${CMAKE_CURRENT_LIST_DIR}/get_test_inputs.sh
    OUTPUT_VARIABLE podio_test_input_data_dir
    RESULT_VARIABLE test_inputs_available
    )
  if (NOT "${test_inputs_available}" STREQUAL "0")
    message(WARNING "Could not get test input files. Will skip some tests that depend on these")
    # Catch cases where the variable is cached but the file no longer exists
    unset(PODIO_TEST_INPUT_DATA_DIR CACHE)
  else()
    message(STATUS "Test inputs stored in: " ${podio_test_input_data_dir})
    set(PODIO_TEST_INPUT_DATA_DIR ${podio_test_input_data_dir} CACHE INTERNAL "input dir for test inputs fetched from remote sources")
    mark_as_advanced(PODIO_TEST_INPUT_DATA_DIR)
  endif()
endif()

# If the variable is cached and defined now, we have inputs and can add the
# legacy file read test
if (DEFINED CACHE{PODIO_TEST_INPUT_DATA_DIR})
  message(STATUS "Using test inputs stored in: "  ${PODIO_TEST_INPUT_DATA_DIR})
  add_executable(read-legacy-files read-legacy-files.cpp)
  target_link_libraries(read-legacy-files TestDataModel TestDataModelDict podio::podioRootIO)
  add_test(NAME read-legacy-files COMMAND read-legacy-files ${PODIO_TEST_INPUT_DATA_DIR}/example.root)

  set_property(TEST read-legacy-files PROPERTY ENVIRONMENT
    LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH}
    ROOT_INCLUDE_PATH=
  )
endif()

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
    GIT_TAG        v3.0.1
    )
  FetchContent_MakeAvailable(Catch2)
  set(CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras ${CMAKE_MODULE_PATH})

  # Disable clang-tidy on external contents
  set_target_properties(Catch2 PROPERTIES CXX_CLANG_TIDY "")

  # Hack around the fact, that the include directories are not declared as
  # SYSTEM for the targets defined this way. Otherwise warnings can still occur
  # in Catch2 code when templates are evaluated (which happens quite a bit)
  get_target_property(CATCH2_IF_INC_DIRS Catch2 INTERFACE_INCLUDE_DIRECTORIES)
  set_target_properties(Catch2 PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${CATCH2_IF_INC_DIRS}")

  # Reset the flags
  set(CMAKE_CXX_FLAGS ${CXX_FLAGS_CMAKE_USED})
endif()


if (TARGET TestDataModelSioBlocks)
  set(sio_dependent_tests
    write_sio.cpp
    read_sio.cpp
    read_and_write_sio.cpp
    write_timed_sio.cpp
    read_timed_sio.cpp
    read_frame_sio.cpp
    write_frame_sio.cpp)
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
set_property(TEST read_frame PROPERTY DEPENDS write_frame_root)

add_executable(check_benchmark_outputs check_benchmark_outputs.cpp)
target_link_libraries(check_benchmark_outputs ROOT::Tree)

add_test(NAME check_benchmark_outputs COMMAND check_benchmark_outputs write_benchmark_root.root read_benchmark_root.root)
set_property(TEST check_benchmark_outputs PROPERTY DEPENDS read_timed write_timed)

if (TARGET read_sio)
  set_property(TEST read_sio PROPERTY DEPENDS write_sio)
  set_property(TEST read_and_write_sio PROPERTY DEPENDS write_sio)
  set_property(TEST read_timed_sio PROPERTY DEPENDS write_timed_sio)
  set_property(TEST read_frame_sio PROPERTY DEPENDS write_frame_sio)

  add_test(NAME check_benchmark_outputs_sio COMMAND check_benchmark_outputs write_benchmark_sio.root read_benchmark_sio.root)
  set_property(TEST check_benchmark_outputs_sio PROPERTY DEPENDS read_timed_sio write_timed_sio)
endif()

add_test( NAME pyunittest COMMAND python -m unittest discover -s ${CMAKE_SOURCE_DIR}/python)
set_property(TEST pyunittest
             PROPERTY ENVIRONMENT
                      LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$<TARGET_FILE_DIR:ROOT::Tree>:$ENV{LD_LIBRARY_PATH}
                      PYTHONPATH=${CMAKE_SOURCE_DIR}/python:$ENV{PYTHONPATH}
                      ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/tests/datamodel
                      SKIP_SIO_TESTS=$<NOT:$<BOOL:${ENABLE_SIO}>>
                      )
set_property(TEST pyunittest PROPERTY DEPENDS write)

# Customize CTest to potentially disable some of the tests with known problems
configure_file(CTestCustom.cmake ${CMAKE_BINARY_DIR}/CTestCustom.cmake)

find_package(Threads REQUIRED)
add_executable(unittest unittest.cpp frame.cpp)
target_link_libraries(unittest PUBLIC TestDataModel PRIVATE Catch2::Catch2WithMain Threads::Threads)

# The unittests are a bit better and they are labelled so we can put together a
# list of labels that we want to ignore
set(filter_tests "")
if (NOT FORCE_RUN_ALL_TESTS)
  if(USE_SANITIZER MATCHES "Address")
    set(filter_tests "~[LEAK-FAIL]~[ASAN-FAIL]")
  elseif(USE_SANITIZER MATCHES "Leak")
    set(filter_tests "~[LEAK-FAIL]")
  elseif(USE_SANITIZER MATCHES "Thread")
    set(filter_tests "~[THREAD-FAIL]")
  endif()
endif()

if (USE_SANITIZER MATCHES "Memory(WithOrigin)?")
  # Automatic test discovery fails with Memory sanitizers due to some issues in
  # Catch2. So in that case we skip the discovery step and simply run the thing
  # directly in the tests.
  if (FORCE_RUN_ALL_TESTS)
    # Unfortunately Memory sanitizer seems to be really unhappy with Catch2 and
    # it fails to succesfully launch the executable and execute any test. Here
    # we just include them in order to have them show up as failing
    add_test(NAME unittest COMMAND unittest ${filter_tests})
    set_property(TEST unittest
      PROPERTY ENVIRONMENT
      LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$<TARGET_FILE_DIR:ROOT::Tree>:$ENV{LD_LIBRARY_PATH}
      )
  endif()
else()
  include(Catch)
  catch_discover_tests(unittest
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
      TEST_PREFIX "UT_" # make it possible to filter easily with -R ^UT
      TEST_SPEC ${filter_tests} # discover only tests that are known to not fail
      PROPERTIES
        ENVIRONMENT
        LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$<TARGET_FILE_DIR:ROOT::Tree>:$ENV{LD_LIBRARY_PATH}
  )
endif()

# Build the extension data model and link it against the upstream model
PODIO_GENERATE_DATAMODEL(extension_model datalayout_extension.yaml ext_headers ext_sources
  UPSTREAM_EDM datamodel:datalayout.yaml
  IO_BACKEND_HANDLERS ${PODIO_IO_HANDLERS}
  OUTPUT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/extension_model)

PODIO_ADD_DATAMODEL_CORE_LIB(ExtensionDataModel "${ext_headers}" "${ext_sources}"
  OUTPUT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/extension_model)
target_link_libraries(ExtensionDataModel PUBLIC TestDataModel)

PODIO_ADD_ROOT_IO_DICT(ExtensionDataModelDict ExtensionDataModel "${ext_headers}" ${CMAKE_CURRENT_SOURCE_DIR}/extension_model/src/selection.xml
  OUTPUT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/extension_model)

PODIO_ADD_SIO_IO_BLOCKS(ExtensionDataModel "${ext_headers}" "${ext_sources}")
