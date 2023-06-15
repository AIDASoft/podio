PODIO_GENERATE_DATAMODEL(datamodel datalayout_old.yaml headers sources
  IO_BACKEND_HANDLERS ${PODIO_IO_HANDLERS}
  OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/datamodel_old
)

PODIO_ADD_DATAMODEL_CORE_LIB(TestDataModel_v1 "${headers}" "${sources}"
  OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/datamodel_old
)

PODIO_ADD_ROOT_IO_DICT(TestDataModel_v1Dict TestDataModel_v1 "${headers}" ${CMAKE_CURRENT_BINARY_DIR}/datamodel_old/src/selection.xml
  OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/datamodel_old
)

PODIO_ADD_SIO_IO_BLOCKS(TestDataModel_v1 "${headers}" "${sources}")

#--- Helper function to create a test in an environment that is suitable to
#--- write data in an old schema version
function(PODIO_CREATE_WRITE_OLD_DATA_TEST sourcefile additional_libs)
  string( REPLACE ".cpp" "" name ${sourcefile} )
  add_executable( ${name} ${sourcefile} )
  add_test(NAME ${name} COMMAND ${name})

  target_link_libraries(${name} PRIVATE TestDataModel_v1 ${additional_libs})
  target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/tests/schema_evolution)

  set_property(TEST ${name}
    PROPERTY ENVIRONMENT
    LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/src:${CMAKE_CURRENT_BINARY_DIR}:$<TARGET_FILE_DIR:ROOT::Tree>:$<$<TARGET_EXISTS:SIO::sio>:$<TARGET_FILE_DIR:SIO::sio>>:$ENV{LD_LIBRARY_PATH}
    PODIO_SIO_BLOCK=${CMAKE_CURRENT_BINARY_DIR}
    ROOT_INCLUDE_PATH=${CMAKE_CURRENT_BINARY_DIR}/datamodel_old:${CMAKE_SOURCE_DIR}/include
  )
endfunction()

add_subdirectory(root_io)
