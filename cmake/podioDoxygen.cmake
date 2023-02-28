find_package(Doxygen)
find_package(Sphinx)

if(DOXYGEN_FOUND AND SPHINX_FOUND)
  # temporarily override the version for doxy generation (for nightly snapshots)
  set(tmp ${podio_VERSION})
  if(DOXYVERSION)
    set(podio_VERSION ${DOXYVERSION})
  endif()
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in
                 ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc
                    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Generating API documentation with Doxygen" VERBATIM)

  set(SPHINX_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sphinx_build)
  add_custom_target(Sphinx ALL
                    COMMAND
                    ${SPHINX_EXE} -M html ${CMAKE_SOURCE_DIR}/doc ${SPHINX_OUTPUT_DIRECTORY}
                    # DEPENDS doxygen
                    )

  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen
    DESTINATION ${CMAKE_INSTALL_DOCDIR}/doxygen
    OPTIONAL)

  # revert version to old value
  set(podio_VERSION ${tmp})
  unset(tmp)
endif()
