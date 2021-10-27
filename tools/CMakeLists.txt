ADD_EXECUTABLE( PodioEventOverview PodioEventOverview.cpp )
TARGET_LINK_LIBRARIES( PodioEventOverview podioRootIO podioSioIO podio )

INSTALL(TARGETS PodioEventOverview DESTINATION ${CMAKE_INSTALL_BINDIR})
