set(podio_PYTHON_INSTALLDIR ${CMAKE_INSTALL_LIBDIR}/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages/podio PARENT_SCOPE)

set(to_install
  podio_class_generator.py
  podio_schema_evolution.py
  )

install(FILES ${to_install} DESTINATION ${podio_PYTHON_INSTALLDIR})

if(ENABLE_SIO)
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/podio
    DESTINATION ${podio_PYTHON_INSTALLDIR}
    REGEX test_.*\\.py$ EXCLUDE  # Do not install test files
    PATTERN __pycache__ EXCLUDE  # Or pythons caches
    )
else()
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/podio
    DESTINATION ${podio_PYTHON_INSTALLDIR}
    REGEX test_.*\\.py$ EXCLUDE  # Do not install test files
    PATTERN __pycache__ EXCLUDE  # Or pythons caches
    REGEX .*sio_.*\\.py$ EXCLUDE  # All things sio related
    )
endif()

# Quality of life improvement to be able to use podio-dump --help
# without having to import ROOT
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/podio_version.py
  DESTINATION ${podio_PYTHON_INSTALLDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/podio_gen
  DESTINATION ${podio_PYTHON_INSTALLDIR}
  REGEX test_.*\\.py$ EXCLUDE  # Do not install test files
  PATTERN __pycache__ EXCLUDE  # Or pythons caches
  )

#--- install templates ---------------------------------------------------------
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/templates
  DESTINATION ${podio_PYTHON_INSTALLDIR})

IF (BUILD_TESTING)
  add_test( NAME pyunittest COMMAND python3 -m unittest discover -s ${PROJECT_SOURCE_DIR}/python/podio)
  PODIO_SET_TEST_ENV(pyunittest)
  add_test( NAME pyunittest_gen COMMAND python3 -m unittest discover -s ${PROJECT_SOURCE_DIR}/python/podio_gen)
  PODIO_SET_TEST_ENV(pyunittest_gen)

  set_property(TEST pyunittest PROPERTY DEPENDS write write_frame_root)
  if (TARGET write_sio)
    set_property(TEST pyunittest PROPERTY DEPENDS write_sio write_frame_sio)
  endif()
  set_tests_properties(pyunittest pyunittest_gen PROPERTIES WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
ENDIF()
