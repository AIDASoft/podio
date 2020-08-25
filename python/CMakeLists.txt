SET(podio_PYTHON_INSTALLDIR python)
SET(podio_PYTHON_INSTALLDIR ${podio_PYTHON_INSTALLDIR} PARENT_SCOPE)
SET(podio_PYTHON_DIR ${CMAKE_CURRENT_LIST_DIR} PARENT_SCOPE)

file(GLOB to_install *py figure.txt)
LIST(FILTER to_install EXCLUDE REGEX "test_.*\\.py$" )
install(FILES ${to_install} DESTINATION ${podio_PYTHON_INSTALLDIR})

#--- install templates ---------------------------------------------------------
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/templates
  DESTINATION ${podio_PYTHON_INSTALLDIR})

