include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB sources *.cc)
file(GLOB headers ${CMAKE_SOURCE_DIR}/include/podio/*.h)
REFLEX_GENERATE_DICTIONARY(podio ${headers} SELECTION selection.xml)
add_library(podioDict SHARED podio.cxx)
add_dependencies(podioDict podio-dictgen)
target_link_libraries(podioDict podio ROOT::Core)
install(TARGETS podioDict DESTINATION lib)

add_library(podio SHARED ${sources})
target_link_libraries(podio ROOT::RIO ROOT::Tree)
install(TARGETS podio DESTINATION lib)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/podio DESTINATION include)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/podioDict.rootmap DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/podio_rdict.pcm DESTINATION lib)
