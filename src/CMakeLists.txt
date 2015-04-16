include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB sources *.cc)

add_library(albers SHARED ${sources})
target_link_libraries(albers ${ROOT_LIBRARIES})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/albers DESTINATION include)
install(TARGETS albers DESTINATION lib)
