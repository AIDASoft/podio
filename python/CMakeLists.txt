SET(podio_PYTHON_INSTALLDIR python)
SET(podio_PYTHON_INSTALLDIR ${podio_PYTHON_INSTALLDIR} PARENT_SCOPE)
SET(podio_PYTHON_DIR ${CMAKE_CURRENT_LIST_DIR} PARENT_SCOPE)

set(to_install
  podio_class_generator.py
  figure.txt
  EventStore.py)

install(FILES ${to_install} DESTINATION ${podio_PYTHON_INSTALLDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/podio
  DESTINATION ${podio_PYTHON_INSTALLDIR}
  REGEX test_.*\\.py$ EXCLUDE  # Do not install test files
  PATTERN __pycache__ EXCLUDE  # Or pythons caches
  )

#--- install templates ---------------------------------------------------------
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/templates
  DESTINATION ${podio_PYTHON_INSTALLDIR})
