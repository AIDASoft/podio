# --- Core podio library and dictionary without I/O
SET(sources
  CollectionIDTable.cc
  GenericParameters.cc
  ASCIIWriter.cc
  EventStore.cc)

# Main Library, no external dependencies
add_library(podio SHARED ${sources})
add_library(podio::podio ALIAS podio)
target_include_directories(podio PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_options(podio PRIVATE -pthread)

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
  ${CMAKE_SOURCE_DIR}/include/podio/UserDataCollection.h
  ${CMAKE_SOURCE_DIR}/include/podio/podioVersion.h
  )
PODIO_GENERATE_DICTIONARY(podioDict ${headers} SELECTION selection.xml
  OPTIONS --library ${CMAKE_SHARED_LIBRARY_PREFIX}podioDict${CMAKE_SHARED_LIBRARY_SUFFIX}
  )
# prevent generating dictionary twice
set_target_properties(podioDict-dictgen PROPERTIES EXCLUDE_FROM_ALL TRUE)

target_sources(podioDict PRIVATE podioDict.cxx)

# --- Root I/O functionality and corresponding dictionary
SET(root_sources
  rootUtils.h
  ROOTWriter.cc
  ROOTReader.cc
  ROOTFrameWriter.cc
  ROOTFrameReader.cc
)

# Shared library
add_library(podioRootIO SHARED ${root_sources})
add_library(podio::podioRootIO ALIAS podioRootIO)
target_link_libraries(podioRootIO PUBLIC podio::podio ROOT::Core ROOT::RIO ROOT::Tree)
target_include_directories(podioRootIO PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# dictionary
add_library(podioRootIODict SHARED)
target_include_directories(podioRootIODict PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(podioRootIODict PUBLIC podio::podioRootIO ROOT::Core ROOT::Tree)
SET(root_headers
  ${CMAKE_SOURCE_DIR}/include/podio/ROOTFrameReader.h
  ${CMAKE_SOURCE_DIR}/include/podio/ROOTFrameWriter.h
  )
PODIO_GENERATE_DICTIONARY(podioRootIODict ${root_headers} SELECTION root_selection.xml
  OPTIONS --library ${CMAKE_SHARED_LIBRARY_PREFIX}podioRootIODict${CMAKE_SHARED_LIBRARY_SUFFIX}
  )
set_target_properties(podioRootIODict-dictgen PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_sources(podioRootIODict PRIVATE podioRootIODict.cxx)

# --- Python EventStore for enabling (legacy) python bindings
SET(python_sources
  IOHelpers.cc
  PythonEventStore.cc
  )

add_library(podioPythonStore SHARED ${python_sources})
target_link_libraries(podioPythonStore podio podioRootIO)
LIST(APPEND INSTALL_LIBRARIES podioPythonStore)

add_library(podioPythonStoreDict SHARED)
target_include_directories(podioPythonStoreDict PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(podioPythonStoreDict PUBLIC podioPythonStore)
SET(python_headers
  ${CMAKE_SOURCE_DIR}/include/podio/PythonEventStore.h
)
PODIO_GENERATE_DICTIONARY(podioPythonStoreDict ${python_headers} SELECTION python_selection.xml
  OPTIONS --library ${CMAKE_SHARED_LIBRARY_PREFIX}podioPythonStoreDict${CMAKE_SHARED_MODULE_SUFFIX})
set_target_properties(podioPythonStoreDict-dictgen PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_sources(podioPythonStoreDict PRIVATE podioPythonStoreDict.cxx)

# --- SIO I/O functionality and corresponding dictionary
if(ENABLE_SIO)
  SET(sio_sources
    SIOReader.cc
    SIOWriter.cc
    SIOBlockUserData.cc
    SIOBlock.cc
    SIOFrameWriter.cc
    SIOFrameReader.cc
    SIOFrameData.cc
    )

  add_library(podioSioIO SHARED ${sio_sources})
  add_library(podio::podioSioIO ALIAS podioSioIO)

  target_include_directories(podioSioIO PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
  target_link_libraries(podioSioIO PUBLIC podio::podio SIO::sio ${CMAKE_DL_LIBS} ${PODIO_FS_LIBS})

  # also make the python EventStore understand SIO
  target_link_libraries(podioPythonStore podioSioIO)
  target_compile_definitions(podioPythonStore PRIVATE PODIO_ENABLE_SIO=1)

  LIST(APPEND INSTALL_LIBRARIES podioSioIO)

  # TODO: dictionary
endif()

# --- Install everything
install(TARGETS podio podioDict podioPythonStoreDict podioRootIO podioRootIODict ${INSTALL_LIBRARIES}
  EXPORT podioTargets
  DESTINATION "${CMAKE_INSTALL_LIBDIR}")

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/podio DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/podioDictDict.rootmap
  ${CMAKE_CURRENT_BINARY_DIR}/libpodioDict_rdict.pcm
  ${CMAKE_CURRENT_BINARY_DIR}/podioRootIODictDict.rootmap
  ${CMAKE_CURRENT_BINARY_DIR}/libpodioRootIODict_rdict.pcm
  ${CMAKE_CURRENT_BINARY_DIR}/podioPythonStoreDictDict.rootmap
  ${CMAKE_CURRENT_BINARY_DIR}/libpodioPythonStoreDict_rdict.pcm
  DESTINATION "${CMAKE_INSTALL_LIBDIR}")
