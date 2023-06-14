# Avoid fetching these everytime cmake is run by caching the directory the first
# time the inputs are fetched or if the expected file does not exist in the
# expected directory
if (NOT DEFINED CACHE{PODIO_TEST_INPUT_DATA_DIR} OR NOT EXISTS ${PODIO_TEST_INPUT_DATA_DIR}/v00-16-05/example_frame.root)
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
