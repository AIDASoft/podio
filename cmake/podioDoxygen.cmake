find_package(Doxygen)
find_package(Sphinx)

if(DOXYGEN_FOUND AND SPHINX_FOUND)
  set(SPHINX_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/doc/_build)

  add_custom_target(documentation
                    COMMAND
                    ${CMAKE_COMMAND} -E env
                    "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/src/:$ENV{LD_LIBRARY_PATH}"
                    "ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/include:$ENV{ROOT_INCLUDE_PATH}"
                    ${SPHINX_BUILD_EXECUTABLE} -M html
                    ${CMAKE_SOURCE_DIR}/doc ${SPHINX_OUTPUT_DIRECTORY}
                    COMMENT "Building documentation" VERBATIM
                    DEPENDS podio::podio podio::podioRootIO
                    )

  install(DIRECTORY ${SPHINX_OUTPUT_DIRECTORY}/
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
    OPTIONAL)
else()
  message(WARNING "Could not generate documentation because either Doxygen or Sphinx could not be found")
endif()
