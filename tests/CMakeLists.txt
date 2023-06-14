foreach( _conf ${CMAKE_CONFIGURATION_TYPES} )
  string(TOUPPER ${_conf} _conf )
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
  set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
endforeach()

# Set the podio_PYTHON_DIR manually here because the macros below expect it
SET(podio_PYTHON_DIR ${CMAKE_SOURCE_DIR}/python)

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

### Define the actual tests
PODIO_DOWNLOAD_LEGACY_INPUTS()

add_executable(check_benchmark_outputs check_benchmark_outputs.cpp)
target_link_libraries(check_benchmark_outputs PRIVATE ROOT::Tree)

add_subdirectory(root_io)
add_subdirectory(sio_io)
add_subdirectory(unittests)

CREATE_PODIO_TEST(ostream_operator.cpp "")
CREATE_PODIO_TEST(write_ascii.cpp "")



# Add tests for storing and retrieving the EDM definitions into the produced
# files
add_test(NAME datamodel_def_store_roundtrip_root COMMAND ${CMAKE_CURRENT_LIST_DIR}/scripts/dumpModelRoundTrip.sh
  ${CMAKE_CURRENT_BINARY_DIR}/root_io/example_frame.root
  datamodel
  ${CMAKE_CURRENT_LIST_DIR}
  )
PODIO_SET_TEST_ENV(datamodel_def_store_roundtrip_root)

# The extension model needs to know about the upstream model for generation
add_test(NAME datamodel_def_store_roundtrip_root_extension COMMAND
  ${CMAKE_CURRENT_LIST_DIR}/scripts/dumpModelRoundTrip.sh
  ${CMAKE_CURRENT_BINARY_DIR}/root_io/example_frame.root
  extension_model
  ${CMAKE_CURRENT_LIST_DIR}/extension_model
  --upstream-edm=datamodel:${CMAKE_CURRENT_LIST_DIR}/datalayout.yaml
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
  add_test(NAME datamodel_def_store_roundtrip_sio COMMAND
    ${CMAKE_CURRENT_LIST_DIR}/scripts/dumpModelRoundTrip.sh
    ${CMAKE_CURRENT_BINARY_DIR}/sio_io/example_frame.sio
    datamodel
    ${CMAKE_CURRENT_LIST_DIR}
    )
  PODIO_SET_TEST_ENV(datamodel_def_store_roundtrip_sio)
  # The extension model needs to know about the upstream model for generation
  add_test(NAME datamodel_def_store_roundtrip_sio_extension COMMAND
    ${CMAKE_CURRENT_LIST_DIR}/scripts/dumpModelRoundTrip.sh
    ${CMAKE_CURRENT_BINARY_DIR}/sio_io/example_frame.sio
    extension_model
    ${CMAKE_CURRENT_LIST_DIR}/extension_model
    --upstream-edm=datamodel:${CMAKE_CURRENT_LIST_DIR}/datalayout.yaml
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

# Customize CTest to potentially disable some of the tests with known problems
configure_file(CTestCustom.cmake ${CMAKE_BINARY_DIR}/CTestCustom.cmake @ONLY)
