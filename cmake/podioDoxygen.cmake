find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in
                 ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/doxy-boot.js.in
                 ${CMAKE_BINARY_DIR}/doxygen/html/doxy-boot.js)
  add_custom_target(doc
                    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Generating API documentation with Doxygen" VERBATIM)

  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen DESTINATION ${CMAKE_INSTALL_DOCDIR} OPTIONAL)
endif()
