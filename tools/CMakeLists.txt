install(PROGRAMS ${CMAKE_CURRENT_LIST_DIR}/podio-dump DESTINATION ${CMAKE_INSTALL_BINDIR})
install(PROGRAMS ${CMAKE_CURRENT_LIST_DIR}/podio-vis DESTINATION ${CMAKE_INSTALL_BINDIR})

# Add a very basic tests here to make sure that podio-dump at least runs
if(BUILD_TESTING)
  # Helper function for easily creating "tests" that simply execute podio-dump
  # with different arguments. Not crashing is considered success.
  #
  # Args:
  #     name        the name of the test
  #     depends_on  the target name of the test that produces the required input file
  function(CREATE_DUMP_TEST name depends_on)
    add_test(NAME ${name} COMMAND ./podio-dump ${ARGN})

    set(SIO_LD_PATH "")
    if (ENABLE_SIO)
      set(SIO_LD_PATH $<TARGET_FILE_DIR:SIO::sio>)
    endif()
    PODIO_SET_TEST_ENV(${name})

    set_tests_properties(${name} PROPERTIES
       WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
       )
    if (depends_on)
      set_tests_properties(${name} PROPERTIES
        DEPENDS ${depends_on}
        )
    endif()
  endfunction()

  CREATE_DUMP_TEST(podio-dump-help _dummy_target_ --help)
  CREATE_DUMP_TEST(podio-dump-root "write_frame_root" ${PROJECT_BINARY_DIR}/tests/root_io/example_frame.root)
  CREATE_DUMP_TEST(podio-dump-detailed-root "write_frame_root" --detailed --category other_events --entries 2:3 ${PROJECT_BINARY_DIR}/tests/root_io/example_frame.root)


  if (DEFINED CACHE{PODIO_TEST_INPUT_DATA_DIR})
    CREATE_DUMP_TEST(podio-dump-root-legacy "" ${PODIO_TEST_INPUT_DATA_DIR}/v00-16-06/example.root)
    CREATE_DUMP_TEST(podio-dump-detailed-root-legacy "" --detailed --entries 2:3 ${PODIO_TEST_INPUT_DATA_DIR}/v00-16-06/example.root)
  endif()

  if (ENABLE_SIO)
    CREATE_DUMP_TEST(podio-dump-sio "write_frame_sio" --entries 4:7 ${PROJECT_BINARY_DIR}/tests/sio_io/example_frame.sio)
    CREATE_DUMP_TEST(podio-dump-detailed-sio "write_frame_sio" --detailed --entries 9 ${PROJECT_BINARY_DIR}/tests/sio_io/example_frame.sio)

    if (DEFINED CACHE{PODIO_TEST_INPUT_DATA_DIR})
      CREATE_DUMP_TEST(podio-dump-sio-legacy "" ${PODIO_TEST_INPUT_DATA_DIR}/v00-16-06/example.sio)
      CREATE_DUMP_TEST(podio-dump-detailed-sio-legacy "" --detailed --entries 2:3 ${PODIO_TEST_INPUT_DATA_DIR}/v00-16-06/example.sio)
    endif()
  endif()

  if (ENABLE_RNTUPLE)
    CREATE_DUMP_TEST(podio-dump-rntuple "write_rntuple" ${PROJECT_BINARY_DIR}/tests/root_io/example_rntuple.root)
    CREATE_DUMP_TEST(podio-dump-rntuple-detailed "write_rntuple" --detailed --category events --entries 1:3 ${PROJECT_BINARY_DIR}/tests/root_io/example_rntuple.root)
  endif()

endif()
