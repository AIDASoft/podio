foreach( _conf ${CMAKE_CONFIGURATION_TYPES} )
  string(TOUPPER ${_conf} _conf )
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
  set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
endforeach()

PODIO_GENERATE_DATAMODEL(datamodel datalayout.yaml headers sources
  IO_BACKEND_HANDLERS ${PODIO_IO_HANDLERS}
  OLD_DESCRIPTION datalayout_old.yaml
  SCHEMA_EVOLUTION schema_evolution.yaml
   )

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

#--- small utility helper function to allow for a more terse definition of tests below
function(CREATE_PODIO_TEST sourcefile additional_libs)
  string( REPLACE ".cpp" "" name ${sourcefile} )
  add_executable( ${name} ${sourcefile} )
  add_test(NAME ${name} COMMAND ${name})

  target_link_libraries(${name} PRIVATE TestDataModel ExtensionDataModel ${additional_libs})
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
  read_frame_root.cpp
  write_frame_root.cpp
  read_frame_legacy_root.cpp
  read_frame_root_multiple.cpp
  )
set(root_libs TestDataModelDict ExtensionDataModelDict podio::podioRootIO)
foreach( sourcefile ${root_dependent_tests} )
  CREATE_PODIO_TEST(${sourcefile} "${root_libs}")
endforeach()

# Avoid fetching these everytime cmake is run by caching the directory the first
# time the inputs are fetched or if the expected file does not exist in the
# expected directory
if (NOT DEFINED CACHE{PODIO_TEST_INPUT_DATA_DIR} OR NOT EXISTS ${PODIO_TEST_INPUT_DATA_DIR}/example.root)
  message("Getting test input files")
  execute_process(
    COMMAND bash ${CMAKE_CURRENT_LIST_DIR}/scripts/get_test_inputs.sh
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
  add_executable(read-legacy-files-root read-legacy-files-root.cpp)
  target_link_libraries(read-legacy-files-root PRIVATE TestDataModel TestDataModelDict podio::podioRootIO)

  # Add a legacy test case based on a base executable and a version for which an
  # input file exists
  macro(ADD_PODIO_LEGACY_TEST version base_test input_file)
    add_test(NAME ${base_test}_${version} COMMAND ${base_test} ${PODIO_TEST_INPUT_DATA_DIR}/${version}/${input_file})
    set_property(TEST ${base_test}_${version} PROPERTY ENVIRONMENT
      LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH}
      # Clear the ROOT_INCLUDE_PATH for the tests, to avoid potential conflicts
      # with existing headers from other installations
      ROOT_INCLUDE_PATH=
    )
  endmacro()

  ADD_PODIO_LEGACY_TEST(v00-13 read-legacy-files-root example.root legacy_test_cases)

  set(legacy_versions v00-16 v00-16-05)
  foreach(version IN LISTS legacy_versions)
    ADD_PODIO_LEGACY_TEST(${version} read-legacy-files-root example.root legacy_test_cases)
    ADD_PODIO_LEGACY_TEST(${version} read_frame_root example_frame.root legacy_test_cases)
  endforeach()

endif()

CREATE_PODIO_TEST(ostream_operator.cpp "")
CREATE_PODIO_TEST(write_ascii.cpp "")

if(USE_EXTERNAL_CATCH2)
  find_package(Catch2 3 REQUIRED)
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
    write_frame_sio.cpp
    read_frame_legacy_sio.cpp)
  set(sio_libs podio::podioSioIO)
  foreach( sourcefile ${sio_dependent_tests} )
    CREATE_PODIO_TEST(${sourcefile} "${sio_libs}")
  endforeach()

  # These need to be linked against TTree explicitly, since it is not done
  # through another library and the TimedReader/Writer decorators are
  # header-only wrappers
  target_link_libraries(write_timed_sio PRIVATE ROOT::Tree)
  target_link_libraries(read_timed_sio PRIVATE ROOT::Tree)
endif()

#--- set some dependencies between the different tests to ensure input generating ones are run first
set_property(TEST read PROPERTY DEPENDS write)
set_property(TEST read-multiple PROPERTY DEPENDS write)
set_property(TEST read_and_write PROPERTY DEPENDS write)
set_property(TEST read_frame_legacy_root PROPERTY DEPENDS write)
set_property(TEST read_timed PROPERTY DEPENDS write_timed)
set_property(TEST read_frame_root PROPERTY DEPENDS write_frame_root)
set_property(TEST read_frame_root_multiple PROPERTY DEPENDS write_frame_root)

add_executable(check_benchmark_outputs check_benchmark_outputs.cpp)
target_link_libraries(check_benchmark_outputs PRIVATE ROOT::Tree)

add_test(NAME check_benchmark_outputs COMMAND check_benchmark_outputs write_benchmark_root.root read_benchmark_root.root)
set_property(TEST check_benchmark_outputs PROPERTY DEPENDS read_timed write_timed)

if (TARGET read_sio)
  set_property(TEST read_sio PROPERTY DEPENDS write_sio)
  set_property(TEST read_and_write_sio PROPERTY DEPENDS write_sio)
  set_property(TEST read_timed_sio PROPERTY DEPENDS write_timed_sio)
  set_property(TEST read_frame_sio PROPERTY DEPENDS write_frame_sio)
  set_property(TEST read_frame_legacy_sio PROPERTY DEPENDS write_sio)

  add_test(NAME check_benchmark_outputs_sio COMMAND check_benchmark_outputs write_benchmark_sio.root read_benchmark_sio.root)
  set_property(TEST check_benchmark_outputs_sio PROPERTY DEPENDS read_timed_sio write_timed_sio)
endif()

add_test( NAME pyunittest COMMAND python -m unittest discover -s ${CMAKE_SOURCE_DIR}/python/podio)
set_property(TEST pyunittest
             PROPERTY ENVIRONMENT
                      LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$<TARGET_FILE_DIR:ROOT::Tree>:$ENV{LD_LIBRARY_PATH}
                      PYTHONPATH=${CMAKE_SOURCE_DIR}/python:$ENV{PYTHONPATH}
                      ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/tests/datamodel:${CMAKE_SOURCE_DIR}/include:$ENV{ROOT_INCLUDE_PATH}
                      SKIP_SIO_TESTS=$<NOT:$<BOOL:${ENABLE_SIO}>>
                      )
set_property(TEST pyunittest PROPERTY DEPENDS write write_frame_root)
if (TARGET write_sio)
  set_property(TEST pyunittest PROPERTY DEPENDS write_sio write_frame_sio)
endif()

# Customize CTest to potentially disable some of the tests with known problems
configure_file(CTestCustom.cmake ${CMAKE_BINARY_DIR}/CTestCustom.cmake @ONLY)

find_package(Threads REQUIRED)
add_executable(unittest unittest.cpp frame.cpp)
target_link_libraries(unittest PUBLIC TestDataModel PRIVATE Catch2::Catch2WithMain Threads::Threads podio::podioRootIO)
if (ENABLE_SIO)
  target_link_libraries(unittest PRIVATE podio::podioSioIO)
endif()

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

option(SKIP_CATCH_DISCOVERY "Skip the Catch2 test discovery" OFF)

if (USE_SANITIZER MATCHES "Memory(WithOrigin)?" OR SKIP_CATCH_DISCOVERY)
  # Automatic test discovery fails with Memory sanitizers due to some issues in
  # Catch2. So in that case we skip the discovery step and simply run the thing
  # directly in the tests.
  if (FORCE_RUN_ALL_TESTS OR SKIP_CATCH_DISCOVERY)
    # Unfortunately Memory sanitizer seems to be really unhappy with Catch2 and
    # it fails to succesfully launch the executable and execute any test. Here
    # we just include them in order to have them show up as failing
    add_test(NAME unittest COMMAND unittest ${filter_tests})
    set_property(TEST unittest
      PROPERTY ENVIRONMENT
        LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$<TARGET_FILE_DIR:ROOT::Tree>:$<$<TARGET_EXISTS:SIO::sio>:$<TARGET_FILE_DIR:SIO::sio>>:$ENV{LD_LIBRARY_PATH}
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
        LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$<TARGET_FILE_DIR:ROOT::Tree>:$<$<TARGET_EXISTS:SIO::sio>:$<TARGET_FILE_DIR:SIO::sio>>:$ENV{LD_LIBRARY_PATH}
  )
endif()

# Add tests for storing and retrieving the EDM definitions into the produced
# files
add_test(datamodel_def_store_roundtrip_root ${CMAKE_CURRENT_LIST_DIR}/scripts/dumpModelRoundTrip.sh
  ${CMAKE_CURRENT_BINARY_DIR}/example_frame.root
  datamodel
  ${CMAKE_CURRENT_LIST_DIR}
  )
# The extension model needs to know about the upstream model for generation
add_test(datamodel_def_store_roundtrip_root_extension
  ${CMAKE_CURRENT_LIST_DIR}/scripts/dumpModelRoundTrip.sh
  ${CMAKE_CURRENT_BINARY_DIR}/example_frame.root
  extension_model
  ${CMAKE_CURRENT_LIST_DIR}/extension_model
  --upstream-edm=datamodel:${CMAKE_CURRENT_LIST_DIR}/datalayout.yaml
  )

# Need the input files that are produced by other tests
set_tests_properties(
    datamodel_def_store_roundtrip_root
    datamodel_def_store_roundtrip_root_extension
  PROPERTIES
    DEPENDS write_frame_root
  )

set(sio_roundtrip_tests "")
if (ENABLE_SIO)
  add_test(datamodel_def_store_roundtrip_sio
    ${CMAKE_CURRENT_LIST_DIR}/scripts/dumpModelRoundTrip.sh
    ${CMAKE_CURRENT_BINARY_DIR}/example_frame.sio
    datamodel
    ${CMAKE_CURRENT_LIST_DIR}
    )
  # The extension model needs to know about the upstream model for generation
  add_test(datamodel_def_store_roundtrip_sio_extension
    ${CMAKE_CURRENT_LIST_DIR}/scripts/dumpModelRoundTrip.sh
    ${CMAKE_CURRENT_BINARY_DIR}/example_frame.sio
    extension_model
    ${CMAKE_CURRENT_LIST_DIR}/extension_model
    --upstream-edm=datamodel:${CMAKE_CURRENT_LIST_DIR}/datalayout.yaml
    )

  set(sio_roundtrip_tests
    datamodel_def_store_roundtrip_sio
    datamodel_def_store_roundtrip_sio_extension
    )

  set_tests_properties(
      ${sio_roundtrip_tests}
    PROPERTIES
      DEPENDS write_frame_sio
    )
endif()

# We need to convert this into a list of arguments that can be used as environment variable
list(JOIN PODIO_IO_HANDLERS " " IO_HANDLERS)

set_tests_properties(
    datamodel_def_store_roundtrip_root
    datamodel_def_store_roundtrip_root_extension
    ${sio_roundtrip_tests}
  PROPERTIES
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    ENVIRONMENT
  "PODIO_BASE=${CMAKE_SOURCE_DIR};IO_HANDLERS=${IO_HANDLERS};ENABLE_SIO=${ENABLE_SIO};PODIO_USE_CLANG_FORMAT=${PODIO_USE_CLANG_FORMAT};LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH};PYTHONPATH=${CMAKE_SOURCE_DIR}/python:$ENV{PYTHONPATH};ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/tests/datamodel:${CMAKE_SOURCE_DIR}/include:$ENV{ROOT_INCLUDE_PATH}"
  )
