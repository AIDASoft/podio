foreach( _conf ${CMAKE_CONFIGURATION_TYPES} )
  string(TOUPPER ${_conf} _conf )
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
  set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${_conf} ${CMAKE_CURRENT_BINARY_DIR} )
endforeach()

# files used in ExtraCode directives
set(extra_code extra_code/component_declarations.cc
  extra_code/declarations.cc
  extra_code/implementations.cc
  extra_code/mutable_declarations.cc
  extra_code/mutable_implementations.cc
)

PODIO_GENERATE_DATAMODEL(datamodel datalayout.yaml headers sources
  IO_BACKEND_HANDLERS ${PODIO_IO_HANDLERS} DEPENDS ${extra_code} VERSION ${${PROJECT_NAME}_VERSION}
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

# Build the interface extension data model and link it against the upstream model
PODIO_GENERATE_DATAMODEL(interface_extension_model datalayout_interface_extension.yaml iext_headers iext_sources
  UPSTREAM_EDM datamodel:datalayout.yaml
  IO_BACKEND_HANDLERS ${PODIO_IO_HANDLERS}
  OUTPUT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/interface_extension_model)

PODIO_ADD_DATAMODEL_CORE_LIB(InterfaceExtensionDataModel "${iext_headers}" "${iext_sources}"
  OUTPUT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/interface_extension_model)
target_link_libraries(InterfaceExtensionDataModel PUBLIC TestDataModel)

PODIO_ADD_ROOT_IO_DICT(InterfaceExtensionDataModelDict InterfaceExtensionDataModel "${iext_headers}" ${CMAKE_CURRENT_SOURCE_DIR}/interface_extension_model/src/selection.xml
  OUTPUT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/interface_extension_model)

PODIO_ADD_SIO_IO_BLOCKS(InterfaceExtensionDataModel "${iext_headers}" "${iext_sources}")

# Add a legacy test case based on a base executable and a version for which an
# input file exists
macro(ADD_PODIO_LEGACY_TEST version base_test input_file)
  ExternalData_Add_Test(legacy_test_cases
    NAME ${base_test}_${version}
    COMMAND ${base_test} DATA{${PROJECT_SOURCE_DIR}/tests/input_files/${input_file}}
  )
  PODIO_SET_TEST_ENV(${base_test}_${version})
endmacro()

set(root_legacy_test_versions
  v00-16
  v00-16-02
  v00-16-05
  v00-16-06
  v00-99
  v01-00
)

add_subdirectory(root_io)
if (ENABLE_SIO)
  set(sio_legacy_test_versions
    v00-16-05
    v00-16-06
    v00-99
    v01-00
  )

  add_subdirectory(sio_io)
endif()
add_subdirectory(unittests)
add_subdirectory(dumpmodel)
add_subdirectory(schema_evolution)

# Tests that don't fit into one of the broad categories above
CREATE_PODIO_TEST(ostream_operator.cpp "")

if (ENABLE_JULIA)
  message(STATUS "Julia Datamodel generation is enabled.")
  PODIO_GENERATE_DATAMODEL(datamodeljulia datalayout.yaml headers sources
    IO_BACKEND_HANDLERS ${PODIO_IO_HANDLERS}
    LANG julia
    )

  PODIO_GENERATE_DATAMODEL(extensionmodeljulia datalayout_extension.yaml ext_headers ext_sources
    UPSTREAM_EDM ../../datamodeljulia:datalayout.yaml
    IO_BACKEND_HANDLERS ${PODIO_IO_HANDLERS}
    OUTPUT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/extension_model
    LANG julia
    )

  find_program(Julia_EXECUTABLE julia)
  if (Julia_EXECUTABLE)
    message(STATUS "Found Julia, will add the Julia tests")
    add_test(NAME julia-unittests COMMAND julia ${CMAKE_CURRENT_SOURCE_DIR}/unittests/unittest.jl)
    set_tests_properties(julia-unittests PROPERTIES
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
      )
  else()
    message(WARNING "Julia not found. Cannot run the Julia tests.")
  endif()
else()
  message(STATUS "Julia Datamodel generation is disabled.")
endif()

# Customize CTest to potentially disable some of the tests with known problems
configure_file(CTestCustom.cmake ${PROJECT_BINARY_DIR}/CTestCustom.cmake @ONLY)
