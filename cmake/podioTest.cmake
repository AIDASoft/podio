#--- small utility helper function to set a consistent test environment for the passed test

function(PODIO_SET_TEST_ENV test)
  # We need to convert this into a list of arguments that can be used as environment variable
  list(JOIN PODIO_IO_HANDLERS " " IO_HANDLERS)
  set_property(TEST ${test}
    PROPERTY ENVIRONMENT
      LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/tests:${CMAKE_BINARY_DIR}/src:$<TARGET_FILE_DIR:ROOT::Tree>:$<$<TARGET_EXISTS:SIO::sio>:$<TARGET_FILE_DIR:SIO::sio>>:$ENV{LD_LIBRARY_PATH}
      PYTHONPATH=${CMAKE_SOURCE_DIR}/python:$ENV{PYTHONPATH}
      PODIO_SIOBLOCK_PATH=${CMAKE_BINARY_DIR}/tests
      ROOT_INCLUDE_PATH=${CMAKE_BINARY_DIR}/tests/datamodel:${CMAKE_SOURCE_DIR}/include
      SKIP_SIO_TESTS=$<NOT:$<BOOL:${ENABLE_SIO}>>
      IO_HANDLERS=${IO_HANDLERS}
      PODIO_USE_CLANG_FORMAT=${PODIO_USE_CLANG_FORMAT}
      PODIO_BASE=${CMAKE_SOURCE_DIR}
      ENABLE_SIO=${ENABLE_SIO}
  )
endfunction()

#--- small utility helper function to allow for a more terse definition of tests below
function(CREATE_PODIO_TEST sourcefile additional_libs)
  string( REPLACE ".cpp" "" name ${sourcefile} )
  add_executable( ${name} ${sourcefile} )
  add_test(NAME ${name} COMMAND ${name})

  target_link_libraries(${name} PRIVATE TestDataModel ExtensionDataModel ${additional_libs})
  PODIO_SET_TEST_ENV(${name})
endfunction()

#--- utility macro to facilitate the downloading of legacy input data
macro(PODIO_DOWNLOAD_LEGACY_INPUTS legacy_versions)
  # Avoid fetching these everytime cmake is run by caching the directory the first
  # time the inputs are fetched or if the expected file does not exist in the
  # expected directory
  if (NOT DEFINED CACHE{PODIO_TEST_INPUT_DATA_DIR} OR NOT EXISTS ${PODIO_TEST_INPUT_DATA_DIR}/v00-16-05/example_frame.root)
    message(STATUS "Getting test input files")
    execute_process(
      COMMAND bash ${CMAKE_SOURCE_DIR}/tests/scripts/get_test_inputs.sh ${legacy_versions}
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
endmacro()
