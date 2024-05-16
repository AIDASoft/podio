# Copy the main .clang-format to here so that the tests can always find it
configure_file(${PROJECT_SOURCE_DIR}/.clang-format ${CMAKE_CURRENT_BINARY_DIR}/.clang-format COPYONLY)

# Add tests for storing and retrieving the EDM definitions into the produced
# files
add_test(NAME datamodel_def_store_roundtrip_root COMMAND ${PROJECT_SOURCE_DIR}/tests/scripts/dumpModelRoundTrip.sh
  ${PROJECT_BINARY_DIR}/tests/root_io/example_frame.root
  datamodel
  ${PROJECT_SOURCE_DIR}/tests
  )
PODIO_SET_TEST_ENV(datamodel_def_store_roundtrip_root)

# The extension model needs to know about the upstream model for generation
add_test(NAME datamodel_def_store_roundtrip_root_extension COMMAND
  ${PROJECT_SOURCE_DIR}/tests/scripts/dumpModelRoundTrip.sh
  ${PROJECT_BINARY_DIR}/tests/root_io/example_frame.root
  extension_model
  ${PROJECT_SOURCE_DIR}/tests/extension_model
  --upstream-edm=datamodel:${PROJECT_SOURCE_DIR}/tests/datalayout.yaml
  )
PODIO_SET_TEST_ENV(datamodel_def_store_roundtrip_root_extension)

# Need the input files that are produced by other tests
set_tests_properties(
    datamodel_def_store_roundtrip_root
    datamodel_def_store_roundtrip_root_extension
  PROPERTIES
    DEPENDS write_frame_root
  )

set(sio_roundtrip_tests "")
if (ENABLE_SIO)
  add_test(NAME datamodel_def_store_roundtrip_sio COMMAND ${PROJECT_SOURCE_DIR}/tests/scripts/dumpModelRoundTrip.sh
    ${PROJECT_BINARY_DIR}/tests/sio_io/example_frame.sio
    datamodel
    ${PROJECT_SOURCE_DIR}/tests
    )
  PODIO_SET_TEST_ENV(datamodel_def_store_roundtrip_sio)
  # The extension model needs to know about the upstream model for generation
  add_test(NAME datamodel_def_store_roundtrip_sio_extension COMMAND ${PROJECT_SOURCE_DIR}/tests/scripts/dumpModelRoundTrip.sh
    ${PROJECT_BINARY_DIR}/tests/sio_io/example_frame.sio
    extension_model
    ${PROJECT_SOURCE_DIR}/tests/extension_model
    --upstream-edm=datamodel:${PROJECT_SOURCE_DIR}/tests/datalayout.yaml
    )
  PODIO_SET_TEST_ENV(datamodel_def_store_roundtrip_sio_extension)

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

set(rntuple_roundtrip_tests "")
if (ENABLE_RNTUPLE)
  add_test(NAME datamodel_def_store_roundtrip_rntuple COMMAND
    ${PROJECT_SOURCE_DIR}/tests/scripts/dumpModelRoundTrip.sh
    ${PROJECT_BINARY_DIR}/tests/root_io/example_rntuple.root
    datamodel
    ${PROJECT_SOURCE_DIR}/tests
  )
  PODIO_SET_TEST_ENV(datamodel_def_store_roundtrip_rntuple)

  add_test(NAME datamodel_def_store_roundtrip_rntuple_extension COMMAND
    ${PROJECT_SOURCE_DIR}/tests/scripts/dumpModelRoundTrip.sh
    ${PROJECT_BINARY_DIR}/tests/root_io/example_rntuple.root
    extension_model
    ${PROJECT_SOURCE_DIR}/tests/extension_model
    --upstream-edm=datamodel:${PROJECT_SOURCE_DIR}/tests/datalayout.yaml
  )
  PODIO_SET_TEST_ENV(datamodel_def_store_roundtrip_rntuple_extension)

  set(rntuple_roundtrip_tests
    datamodel_def_store_roundtrip_rntuple
    datamodel_def_store_roundtrip_rntuple_extension
  )

  set_tests_properties(
      ${rntuple_roundtrip_tests}
    PROPERTIES
      DEPENDS write_rntuple
  )

endif()

set_tests_properties(
    datamodel_def_store_roundtrip_root
    datamodel_def_store_roundtrip_root_extension
    ${sio_roundtrip_tests}
    ${rntuple_roundtrip_tests}
  PROPERTIES
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
