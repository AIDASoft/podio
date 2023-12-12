#--- small utility helper function to set a consistent test environment for the passed test

function(PODIO_SET_TEST_ENV test)
  # We need to convert this into a list of arguments that can be used as environment variable
  list(JOIN PODIO_IO_HANDLERS " " IO_HANDLERS)
  set(test_environment
    LD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/tests:${PROJECT_BINARY_DIR}/src:$<TARGET_FILE_DIR:ROOT::Tree>:$<$<TARGET_EXISTS:SIO::sio>:$<TARGET_FILE_DIR:SIO::sio>>:$ENV{LD_LIBRARY_PATH}
      PYTHONPATH=${PROJECT_SOURCE_DIR}/python:$ENV{PYTHONPATH}
      PODIO_SIOBLOCK_PATH=${PROJECT_BINARY_DIR}/tests
      ROOT_INCLUDE_PATH=${PROJECT_BINARY_DIR}/tests/datamodel:${PROJECT_SOURCE_DIR}/include
      SKIP_SIO_TESTS=$<NOT:$<BOOL:${ENABLE_SIO}>>
      IO_HANDLERS=${IO_HANDLERS}
      PODIO_USE_CLANG_FORMAT=${PODIO_USE_CLANG_FORMAT}
      PODIO_BASE=${PROJECT_SOURCE_DIR}
      ENABLE_SIO=${ENABLE_SIO}
      PODIO_BUILD_BASE=${PROJECT_BINARY_DIR}
  )
  set_property(TEST ${test}
    PROPERTY ENVIRONMENT "${test_environment}"
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
