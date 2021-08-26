ADD_EXECUTABLE( PodioEventOverview PodioEventOverview.cpp )
TARGET_LINK_LIBRARIES( PodioEventOverview podioRootIO podioSioIO podio )

ADD_EXECUTABLE( PodioReader PodioReader.cpp )
TARGET_LINK_LIBRARIES( PodioReader podioRootIO podioSioIO podio )





