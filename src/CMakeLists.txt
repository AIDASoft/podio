# This is needed for older ROOTs which do not understand
# target usage requirements

SET(sources
  CollectionIDTable.cc
  GenericParameters.cc
  ASCIIWriter.cc
  EventStore.cc
)

SET(root_sources
  rootUtils.h
  ROOTWriter.cc
  ROOTReader.cc
  PythonEventStore.cc
)

SET(sio_sources
  SIOReader.cc
  SIOWriter.cc
  SIOBlockUserData.cc
  SIOBlock.cc
)

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
  ${CMAKE_SOURCE_DIR}/include/podio/UserDataCollection.h
  )
PODIO_GENERATE_DICTIONARY(podioDict ${headers} SELECTION selection.xml
  OPTIONS --library ${CMAKE_SHARED_LIBRARY_PREFIX}podioDict${CMAKE_SHARED_LIBRARY_SUFFIX}
  )
# prevent generating dictionary twice
set_target_properties(podioDict-dictgen PROPERTIES EXCLUDE_FROM_ALL TRUE)

target_sources(podioDict PRIVATE podioDict.cxx)

# SIO I/O library
if(ENABLE_SIO)
  add_library(podioSioIO SHARED ${sio_sources})
  add_library(podio::podioSioIO ALIAS podioSioIO)

  target_include_directories(podioSioIO PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
  target_link_libraries(podioSioIO PUBLIC podio::podio SIO::sio ${CMAKE_DL_LIBS} ${PODIO_FS_LIBS})

  LIST(APPEND INSTALL_LIBRARIES podioSioIO)
endif()

install(TARGETS podio podioDict podioRootIO ${INSTALL_LIBRARIES}
  EXPORT podioTargets
  DESTINATION "${CMAKE_INSTALL_LIBDIR}")

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/podio DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/podioDictDict.rootmap
  ${CMAKE_CURRENT_BINARY_DIR}/libpodioDict_rdict.pcm
  DESTINATION "${CMAKE_INSTALL_LIBDIR}")
