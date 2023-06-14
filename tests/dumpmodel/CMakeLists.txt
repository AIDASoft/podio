# Add tests for storing and retrieving the EDM definitions into the produced
# files
add_test(NAME datamodel_def_store_roundtrip_root COMMAND ${CMAKE_SOURCE_DIR}/tests/scripts/dumpModelRoundTrip.sh
  ${CMAKE_BINARY_DIR}/tests/root_io/example_frame.root
  datamodel
  ${CMAKE_SOURCE_DIR}/tests
  )
PODIO_SET_TEST_ENV(datamodel_def_store_roundtrip_root)

# The extension model needs to know about the upstream model for generation
add_test(NAME datamodel_def_store_roundtrip_root_extension COMMAND
  ${CMAKE_SOURCE_DIR}/tests/scripts/dumpModelRoundTrip.sh
  ${CMAKE_BINARY_DIR}/tests/root_io/example_frame.root
  extension_model
  ${CMAKE_SOURCE_DIR}/tests/extension_model
  --upstream-edm=datamodel:${CMAKE_SOURCE_DIR}/tests/datalayout.yaml
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
  add_test(NAME datamodel_def_store_roundtrip_sio COMMAND ${CMAKE_SOURCE_DIR}/tests/scripts/dumpModelRoundTrip.sh
    ${CMAKE_BINARY_DIR}/tests/sio_io/example_frame.sio
    datamodel
    ${CMAKE_SOURCE_DIR}/tests
    )
  PODIO_SET_TEST_ENV(datamodel_def_store_roundtrip_sio)
  # The extension model needs to know about the upstream model for generation
  add_test(NAME datamodel_def_store_roundtrip_sio_extension COMMAND ${CMAKE_SOURCE_DIR}/tests/scripts/dumpModelRoundTrip.sh
    ${CMAKE_BINARY_DIR}/tests/sio_io/example_frame.sio
    extension_model
    ${CMAKE_SOURCE_DIR}/tests/extension_model
    --upstream-edm=datamodel:${CMAKE_SOURCE_DIR}/tests/datalayout.yaml
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

set_tests_properties(
    datamodel_def_store_roundtrip_root
    datamodel_def_store_roundtrip_root_extension
    ${sio_roundtrip_tests}
  PROPERTIES
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
