file(GLOB to_install *py figure.txt)

install(FILES ${to_install} DESTINATION python)

#--- install templates ---------------------------------------------------------
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/templates
  DESTINATION python)

