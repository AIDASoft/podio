include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB pcms ${CMAKE_CURRENT_BINARY_DIR}/*.pcm ${CMAKE_CURRENT_BINARY_DIR}/*.rootmap)
file(GLOB sources *.cc)
file(GLOB headers ../include/podio/*.h)
REFLEX_GENERATE_DICTIONARY(podio ${headers} SELECTION selection.xml)
add_library(podioDict SHARED podio.cxx)
target_link_libraries(podioDict podio ${ROOT_LIBRARIES})
install(TARGETS podioDict DESTINATION lib)

add_library(podio SHARED ${sources})
target_link_libraries(podio ${ROOT_LIBRARIES})
install(TARGETS podio DESTINATION lib)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/podio DESTINATION include)
install(FILES ${pcms} DESTINATION lib)
