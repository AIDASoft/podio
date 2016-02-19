include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB sources *.cc)
file(GLOB headers ${CMAKE_SOURCE_DIR}/include/podio/*.h)
REFLEX_GENERATE_DICTIONARY(podio ${headers} SELECTION selection.xml)
add_library(podioDict SHARED podio.cxx)
target_link_libraries(podioDict podio ${ROOT_LIBRARIES})
install(TARGETS podioDict DESTINATION lib)

add_library(podio SHARED ${sources})
target_link_libraries(podio ${ROOT_LIBRARIES})
install(TARGETS podio DESTINATION lib)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/podio DESTINATION include)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/podioDict.rootmap DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/podio_rdict.pcm DESTINATION lib)
