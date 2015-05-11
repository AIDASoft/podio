include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB sources *.cc)

add_library(podio SHARED ${sources})
target_link_libraries(podio ${ROOT_LIBRARIES})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/podio DESTINATION include)
install(TARGETS podio DESTINATION lib)
