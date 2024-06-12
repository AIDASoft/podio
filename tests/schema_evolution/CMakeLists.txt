PODIO_GENERATE_DATAMODEL(datamodel datalayout_new.yaml headers sources
  IO_BACKEND_HANDLERS ${PODIO_IO_HANDLERS}
  OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/datamodel_new
  OLD_DESCRIPTION ${PROJECT_SOURCE_DIR}/tests/datalayout.yaml
  SCHEMA_EVOLUTION schema_evolution.yaml
)

PODIO_ADD_DATAMODEL_CORE_LIB(TestDataModel_v3 "${headers}" "${sources}"
  OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/datamodel_new
)

PODIO_ADD_ROOT_IO_DICT(TestDataModel_v3Dict TestDataModel_v3 "${headers}" ${CMAKE_CURRENT_BINARY_DIR}/datamodel_new/src/selection.xml
  OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/datamodel_new
)

PODIO_ADD_SIO_IO_BLOCKS(TestDataModel_v3 "${headers}" "${sources}")

#--- Helper function to create a test in an environment that is suitable to
#--- read data in a new schema version
function(PODIO_CREATE_READ_NEW_DATA_TEST sourcefile additional_libs)
  string( REPLACE ".cpp" "" name ${sourcefile} )
  add_executable( ${name} ${sourcefile} )
  add_test(NAME ${name} COMMAND ${name})

  target_link_libraries(${name} PRIVATE TestDataModel_v3 ${additional_libs})
  target_include_directories(${name} PRIVATE ${PROJECT_SOURCE_DIR}/tests/schema_evolution)

  set_property(TEST ${name}
    PROPERTY ENVIRONMENT
    LD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/src:${PROJECT_BINARY_DIR}/tests/schema_evolution:$<TARGET_FILE_DIR:ROOT::Tree>:$<$<TARGET_EXISTS:SIO::sio>:$<TARGET_FILE_DIR:SIO::sio>>:$ENV{LD_LIBRARY_PATH}
    PODIO_SIO_BLOCK=${CMAKE_CURRENT_BINARY_DIR}
    ROOT_INCLUDE_PATH=${PROJECT_BINARY_DIR}/tests/schema_evolution/datamodel_new:${PROJECT_SOURCE_DIR}/include
  )
endfunction()

add_test(
  NAME schema-evolution-script
  COMMAND ${PROJECT_SOURCE_DIR}/python/podio_schema_evolution.py
    ${CMAKE_CURRENT_SOURCE_DIR}/datalayout_new.yaml
    ${PROJECT_SOURCE_DIR}/tests/datalayout.yaml
)

add_test(
  NAME schema-evolution-script-with-evol
  COMMAND
    ${PROJECT_SOURCE_DIR}/python/podio_schema_evolution.py
    --evo ${CMAKE_CURRENT_SOURCE_DIR}/schema_evolution.yaml
    ${CMAKE_CURRENT_SOURCE_DIR}/datalayout_new.yaml
    ${PROJECT_SOURCE_DIR}/tests/datalayout.yaml
)

add_subdirectory(root_io)
