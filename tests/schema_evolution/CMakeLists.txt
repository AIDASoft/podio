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
