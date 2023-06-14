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
      LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/tests:${CMAKE_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH}
      # Clear the ROOT_INCLUDE_PATH for the tests, to avoid potential conflicts
      # with existing headers from other installations
      ROOT_INCLUDE_PATH=
      # Only pick up this build for testing
      PODIO_SIOBLOCK_PATH=${CMAKE_BINARY_DIR}/tests
    )
endfunction()

include(legacy_input_download.cmake)

add_executable(check_benchmark_outputs check_benchmark_outputs.cpp)
target_link_libraries(check_benchmark_outputs PRIVATE ROOT::Tree)

add_subdirectory(root_io)
add_subdirectory(sio_io)
add_subdirectory(unittests)

CREATE_PODIO_TEST(ostream_operator.cpp "")
CREATE_PODIO_TEST(write_ascii.cpp "")


add_test( NAME pyunittest COMMAND python3 -m unittest discover -s ${CMAKE_SOURCE_DIR}/python/podio)
set_property(TEST pyunittest
             PROPERTY ENVIRONMENT
                      LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/src:$<TARGET_FILE_DIR:ROOT::Tree>:$ENV{LD_LIBRARY_PATH}
                      PYTHONPATH=${CMAKE_SOURCE_DIR}/python:$ENV{PYTHONPATH}
                      ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/tests/datamodel:${CMAKE_SOURCE_DIR}/include:$ENV{ROOT_INCLUDE_PATH}
                      SKIP_SIO_TESTS=$<NOT:$<BOOL:${ENABLE_SIO}>>
                      # Only pick up this build for testing
                      PODIO_SIOBLOCK_PATH=${CMAKE_CURRENT_BINARY_DIR}
                      )
set_property(TEST pyunittest PROPERTY DEPENDS write write_frame_root)
if (TARGET write_sio)
  set_property(TEST pyunittest PROPERTY DEPENDS write_sio write_frame_sio)
endif()

# Add tests for storing and retrieving the EDM definitions into the produced
# files
add_test(datamodel_def_store_roundtrip_root ${CMAKE_CURRENT_LIST_DIR}/scripts/dumpModelRoundTrip.sh
  ${CMAKE_CURRENT_BINARY_DIR}/root_io/example_frame.root
  datamodel
  ${CMAKE_CURRENT_LIST_DIR}
  )
# The extension model needs to know about the upstream model for generation
add_test(datamodel_def_store_roundtrip_root_extension
  ${CMAKE_CURRENT_LIST_DIR}/scripts/dumpModelRoundTrip.sh
  ${CMAKE_CURRENT_BINARY_DIR}/root_io/example_frame.root
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
    ${CMAKE_CURRENT_BINARY_DIR}/sio_io/example_frame.sio
    datamodel
    ${CMAKE_CURRENT_LIST_DIR}
    )
  # The extension model needs to know about the upstream model for generation
  add_test(datamodel_def_store_roundtrip_sio_extension
    ${CMAKE_CURRENT_LIST_DIR}/scripts/dumpModelRoundTrip.sh
    ${CMAKE_CURRENT_BINARY_DIR}/sio_io/example_frame.sio
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
  "PODIO_BASE=${CMAKE_SOURCE_DIR};IO_HANDLERS=${IO_HANDLERS};ENABLE_SIO=${ENABLE_SIO};PODIO_USE_CLANG_FORMAT=${PODIO_USE_CLANG_FORMAT};LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/tests:${CMAKE_BINARY_DIR}/src:$ENV{LD_LIBRARY_PATH};PYTHONPATH=${CMAKE_SOURCE_DIR}/python:$ENV{PYTHONPATH};ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/tests/datamodel:${CMAKE_SOURCE_DIR}/include:$ENV{ROOT_INCLUDE_PATH};PODIO_SIOBLOCK_PATH=${CMAKE_BINARY_DIR}/tests"
  )

# Customize CTest to potentially disable some of the tests with known problems
configure_file(CTestCustom.cmake ${CMAKE_BINARY_DIR}/CTestCustom.cmake @ONLY)
