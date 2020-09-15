# This is needed for older ROOTs which do not understand
# target usage requirements

file(GLOB sources *.cc)
SET(root_sources ${sources})

# --- Store the sources for sio into a separate list
if(SIO_HANDLERS)
  SET(sio_sources ${sources})
  LIST(FILTER sio_sources INCLUDE REGEX SIO.*)
endif()

# Remove SIO and ROOT related things from the core library
LIST(FILTER sources EXCLUDE REGEX SIO.*)
LIST(FILTER sources EXCLUDE REGEX ROOT.*|PythonEventStore.* )
LIST(FILTER root_sources INCLUDE REGEX ROOT.*|PythonEventStore.* )

# Main Library, no external dependencies
add_library(podio SHARED ${sources})
add_library(podio::podio ALIAS podio)
target_include_directories(podio PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_options(podio PRIVATE -pthread)

# Root dependency, mostly IO
add_library(podioRootIO SHARED ${root_sources})
add_library(podio::podioRootIO ALIAS podioRootIO)
target_link_libraries(podioRootIO PUBLIC podio::podio ROOT::Core ROOT::RIO ROOT::Tree)
target_include_directories(podioRootIO PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Dict Library
add_library(podioDict SHARED)
add_library(podio::podioDict ALIAS podioDict)
target_include_directories(podioDict PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(podioDict PUBLIC podio::podio ROOT::Core ROOT::Tree)

SET(headers
  ${CMAKE_SOURCE_DIR}/include/podio/CollectionBase.h
  ${CMAKE_SOURCE_DIR}/include/podio/CollectionIDTable.h
  ${CMAKE_SOURCE_DIR}/include/podio/EventStore.h
  ${CMAKE_SOURCE_DIR}/include/podio/ICollectionProvider.h
  ${CMAKE_SOURCE_DIR}/include/podio/IReader.h
  ${CMAKE_SOURCE_DIR}/include/podio/ObjectID.h
  ${CMAKE_SOURCE_DIR}/include/podio/PythonEventStore.h
  )
PODIO_GENERATE_DICTIONARY(podioDict ${headers} SELECTION selection.xml
  OPTIONS --library ${CMAKE_SHARED_LIBRARY_PREFIX}podioDict${CMAKE_SHARED_LIBRARY_SUFFIX}
  )
# prevent generating dictionary twice
set_target_properties(podioDict-dictgen PROPERTIES EXCLUDE_FROM_ALL TRUE)

target_sources(podioDict PRIVATE podioDict.cxx)

# SIO I/O library
if(SIO_HANDLERS)
add_library(podioSioIO SHARED ${sio_sources})
add_library(podio::podioSioIO ALIAS podioSioIO)
target_include_directories(podio PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(podioSioIO PUBLIC podio::podio ${SIO_LIBRARIES} ${CMAKE_DL_LIBS})

  # Install the Targets and Headers
  install(TARGETS podio podioDict podioRootIO podioSioIO
    EXPORT podioTargets
    DESTINATION "${CMAKE_INSTALL_LIBDIR}")
else()
  # Install the Targets and Headers
  install(TARGETS podio podioDict podioRootIO
    EXPORT podioTargets
    DESTINATION "${CMAKE_INSTALL_LIBDIR}")
endif()



install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/podio DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/podioDictDict.rootmap
  ${CMAKE_CURRENT_BINARY_DIR}/libpodioDict_rdict.pcm
  DESTINATION "${CMAKE_INSTALL_LIBDIR}")
