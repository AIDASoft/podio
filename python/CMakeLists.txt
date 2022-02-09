SET(podio_PYTHON_INSTALLDIR python)
SET(podio_PYTHON_INSTALLDIR ${podio_PYTHON_INSTALLDIR} PARENT_SCOPE)
SET(podio_PYTHON_DIR ${CMAKE_CURRENT_LIST_DIR} PARENT_SCOPE)

file(GLOB to_install *.py figure.txt)

# remove test_*.py file from being installed
foreach(file_path ${to_install})
  get_filename_component(file_name ${file_path} NAME)
  string(REGEX MATCH test_.*\\.py$ FOUND_PY_TEST ${file_name})
  if (NOT "${FOUND_PY_TEST}" STREQUAL "")
    list(REMOVE_ITEM to_install "${file_path}")
  endif()
endforeach()

install(FILES ${to_install} DESTINATION ${podio_PYTHON_INSTALLDIR})

#--- install templates ---------------------------------------------------------
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/templates
  DESTINATION ${podio_PYTHON_INSTALLDIR})
