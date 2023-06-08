install(PROGRAMS ${CMAKE_CURRENT_LIST_DIR}/podio-dump DESTINATION ${CMAKE_INSTALL_BINDIR})

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

    set_property(TEST ${name}
      PROPERTY ENVIRONMENT
      LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/tests:${CMAKE_BINARY_DIR}/src:$<TARGET_FILE_DIR:ROOT::Tree>:${SIO_LD_PATH}:$ENV{LD_LIBRARY_PATH}
      PYTHONPATH=${CMAKE_SOURCE_DIR}/python:$ENV{PYTHONPATH}
      ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/tests/datamodel:${CMAKE_SOURCE_DIR}/include:$ENV{ROOT_INCLUDE_PATH}
      # Only pick up this build for testing
      PODIO_SIOBLOCK_PATH=${CMAKE_BINARY_DIR}/tests
      )

    set_tests_properties(${name} PROPERTIES
      DEPENDS ${depends_on}
       WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
       )
  endfunction()

  CREATE_DUMP_TEST(podio-dump-help _dummy_target_ --help)
  CREATE_DUMP_TEST(podio-dump-root-legacy "write" ${CMAKE_BINARY_DIR}/tests/example.root)
  CREATE_DUMP_TEST(podio-dump-root "write_frame_root" ${CMAKE_BINARY_DIR}/tests/example_frame.root)
  CREATE_DUMP_TEST(podio-dump-detailed-root "write_frame_root" --detailed --category other_events --entries 2:3 ${CMAKE_BINARY_DIR}/tests/example_frame.root)
  CREATE_DUMP_TEST(podio-dump-detailed-root-legacy "write" --detailed --entries 2:3 ${CMAKE_BINARY_DIR}/tests/example.root)

  if (ENABLE_SIO)
    CREATE_DUMP_TEST(podio-dump-sio-legacy "write_sio" ${CMAKE_BINARY_DIR}/tests/example.sio)
    CREATE_DUMP_TEST(podio-dump-sio "write_frame_sio" --entries 4:7 ${CMAKE_BINARY_DIR}/tests/example_frame.sio)
    CREATE_DUMP_TEST(podio-dump-detailed-sio "write_frame_sio" --detailed --entries 9 ${CMAKE_BINARY_DIR}/tests/example_frame.sio)
    CREATE_DUMP_TEST(podio-dump-detailed-sio-legacy "write_sio" --detailed --entries 9 ${CMAKE_BINARY_DIR}/tests/example.sio)
  endif()

endif()
