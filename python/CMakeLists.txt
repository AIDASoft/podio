set(to_install
  podio_class_generator.py
  podio_schema_evolution.py
  )

install(FILES ${to_install} DESTINATION ${podio_PYTHON_INSTALLDIR})

set(python_excludes REGEX test_.*\\.py$ PATTERN __pycache__ EXCLUDE)

if (NOT ENABLE_SIO)
  list(APPEND python_excludes REGEX .*sio_.*\\.py$ EXCLUDE)
endif()

if (NOT ENABLE_ROOT)
  list(APPEND python_excludes REGEX .*root_.*\\.py$ EXCLUDE)
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/podio
  DESTINATION ${podio_PYTHON_INSTALLDIR}
  ${python_excludes}
  )

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
  if (ENABLE_ROOT)
    add_test( NAME pyunittest COMMAND python3 -m unittest discover -s ${PROJECT_SOURCE_DIR}/python/podio)
    PODIO_SET_TEST_ENV(pyunittest PYTHON)
    if (TARGET write_frame_root)
      set_property(TEST pyunittest APPEND PROPERTY FIXTURES_REQUIRED podio_write_root_fixture)
    endif()
    if (TARGET write_frame_sio)
      set_property(TEST pyunittest APPEND PROPERTY FIXTURES_REQUIRED podio_write_sio_fixture)
    endif()
    set_tests_properties(pyunittest PROPERTIES WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
  endif()
  
  add_test( NAME pyunittest_gen COMMAND python3 -m unittest discover -s ${PROJECT_SOURCE_DIR}/python/podio_gen)
  PODIO_SET_TEST_ENV(pyunittest_gen PYTHON)
  set_tests_properties(pyunittest_gen PROPERTIES WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
  
  if (ENABLE_ROOT)
    find_program(strace_program
      NAMES strace
      DOC "strace program to trace system calls"
    )

    option(PODIO_RUN_STRACE_TEST "Run strace test" OFF)
    if(PODIO_RUN_STRACE_TEST)
      if (NOT strace_program)
        message(WARNING "strace not found, skipping test_strace")
      else()
        message(STATUS "strace found: ${strace_program}")
        add_test(NAME test_strace
          COMMAND bash -c "
            ${strace_program} -e open,openat,creat -f -o ${CMAKE_CURRENT_BINARY_DIR}/strace.log \
              python3 ${CMAKE_CURRENT_SOURCE_DIR}/podio/test_strace.py && \
            lines=\$(wc -l < ${CMAKE_CURRENT_BINARY_DIR}/strace.log) && \
            if [ \$lines -ge 16000 ]; then
              echo \"Too many lines in strace.log (\$lines), test failed!\";
              exit 1;
            fi"
        )
        PODIO_SET_TEST_ENV(test_strace)
      endif()
    endif()
  endif()
ENDIF()
