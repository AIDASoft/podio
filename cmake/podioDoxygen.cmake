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

  add_custom_target(doxygen
                    ${DOXYGEN_EXECUTABLE} ${DOXYGEN_FILE}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Generating API documentation with Doxygen" VERBATIM)

  add_custom_target(pybindings_doc
                    COMMAND
                    ${SPHINX_API_DOC_EXECUTABLE} --force -o ${CMAKE_CURRENT_SOURCE_DIR}/doc/py_api
                    ${CMAKE_SOURCE_DIR}/python ${CMAKE_SOURCE_DIR}/python/podio/test_*.py
                    COMMENT "Generating python bindigns documentation" VERBATIM
                    )

  set(SPHINX_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sphinx_build)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/conf.py.in
                 ${CMAKE_CURRENT_SOURCE_DIR}/doc/conf.py)

  add_custom_target(Sphinx
                    COMMAND
                    ${CMAKE_COMMAND} -E env "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/src/:$ENV{LD_LIBRARY_PATH} ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/include"
                    ${SPHINX_BUILD_EXECUTABLE} -b html
                    -Dbreathe_projects.PODIO=${DOXYGEN_OUTPUT_DIRECTORY}/xml
                    ${CMAKE_SOURCE_DIR}/doc ${SPHINX_OUTPUT_DIRECTORY}
                    DEPENDS doxygen pybindings_doc
                    COMMENT "Building sphinx documentation" VERBATIM
                    )

  install(DIRECTORY ${SPHINX_OUTPUT_DIRECTORY}
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
    OPTIONAL)

  # revert version to old value
  set(podio_VERSION ${tmp})
  unset(tmp)
else()
  message(WARNING "Could not generate documentation because either Doxygen or Sphinx could not be found")
endif()
