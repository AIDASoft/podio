include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB sources *.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/podio/*.h)
REFLEX_GENERATE_DICTIONARY(podio ${headers} SELECTION selection.xml)
add_library(podioDict SHARED podio.cxx)
add_dependencies(podioDict podio-dictgen)
target_link_libraries(podioDict podio ${ROOT_LIBRARIES})
install(TARGETS podioDict
  EXPORT podioTargets
  DESTINATION ${CMAKE_INSTALL_LIBDIR})

add_library(podio SHARED ${sources})
target_link_libraries(podio ${ROOT_LIBRARIES})
install(TARGETS podio
  EXPORT podioTargets
  DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/podio DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/podioDict.rootmap DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/podio_rdict.pcm DESTINATION ${CMAKE_INSTALL_LIBDIR})
