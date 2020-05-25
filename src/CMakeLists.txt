# This is needed for older ROOTs which do not understand
# target usage requirements

file(GLOB sources *.cc)
SET(root_sources ${sources})

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
  OPTIONS --library ${CMAKE_SHARED_LIBRARY_PREFIX}podio${CMAKE_SHARED_LIBRARY_SUFFIX}
  )
# prevent generating dictionary twice
set_target_properties(podioDict-dictgen PROPERTIES EXCLUDE_FROM_ALL TRUE)

# check if the linker supports as needed or not
set(STORE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
include(CheckCCompilerFlag)
set(CMAKE_REQUIRED_FLAGS "-Wl,--no-as-needed")
check_c_compiler_flag("" TEST_NO_AS_NEEDED)
if(TEST_NO_AS_NEEDED)
    message(STATUS "Linker supports: -Wl,--no-as-needed")
else()
    message(STATUS "Linker does not support: -Wl,--no-as-needed")
endif()
set(CMAKE_REQUIRED_FLAGS "-Wl,--as-needed")
check_c_compiler_flag("" TEST_AS_NEEDED)
if(TEST_AS_NEEDED)
    message(STATUS "Linker supports: -Wl,--as-needed")
else()
    message(STATUS "Linker does not support: -Wl,--as-needed")
endif()
set(CMAKE_REQUIRED_FLAGS ${STORE_REQUIRED_FLAGS})

target_sources(podioDict PRIVATE podioDict.cxx)
# ROOT IO without the Dict does not work, so we link RootIO against the Dict
# need to disable as needed if linker supports it
if(TEST_NO_AS_NEEDED AND TEST_AS_NEEDED)
  target_link_libraries(podioRootIO
    PRIVATE
      -Wl,--push-state,--no-as-needed podioDict -Wl,--pop-state
    INTERFACE
      podioDict
     )
 else()
   target_link_libraries(podioRootIO PUBLIC podioDict)
 endif()

# Install the Targets and Headers
install(TARGETS podio podioDict podioRootIO
  EXPORT podioTargets
  DESTINATION "${CMAKE_INSTALL_LIBDIR}")

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/podio DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/podioDictDict.rootmap
  ${CMAKE_CURRENT_BINARY_DIR}/libpodio_rdict.pcm
  DESTINATION "${CMAKE_INSTALL_LIBDIR}")
