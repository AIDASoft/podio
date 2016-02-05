file(GLOB to_install *py figure.txt)

install(FILES ${to_install} DESTINATION python)
