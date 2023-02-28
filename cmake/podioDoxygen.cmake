find_package(Doxygen)
find_package(Sphinx)

if(DOXYGEN_FOUND AND SPHINX_FOUND)
  # temporarily override the version for doxy generation (for nightly snapshots)
  set(tmp ${podio_VERSION})
  if(DOXYVERSION)
    set(podio_VERSION ${DOXYVERSION})
  endif()

  set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/doxygen)
  file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIRECTORY}) # Doxygen doesn't create this for us

  set(DOXYGEN_FILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in
                 ${DOXYGEN_FILE} @ONLY)

  add_custom_target(doxygen ALL
                    ${DOXYGEN_EXECUTABLE} ${DOXYGEN_FILE}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Generating API documentation with Doxygen" VERBATIM)

  set(SPHINX_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sphinx_build)
  add_custom_target(Sphinx ALL
                    COMMAND
                    ${SPHINX_EXE} -b html
                    -Dbreathe_projects.PODIO=${DOXYGEN_OUTPUT_DIRECTORY}/xml
                    ${CMAKE_SOURCE_DIR}/doc ${SPHINX_OUTPUT_DIRECTORY}
                    DEPENDS doxygen
                    )

  install(DIRECTORY ${SPHINX_OUTPUT_DIRECTORY}
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
    OPTIONAL)

  # revert version to old value
  set(podio_VERSION ${tmp})
  unset(tmp)
endif()
