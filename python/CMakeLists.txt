file(GLOB to_install *py figure.txt)

install(PROGRAMS ${to_install} DESTINATION python)
