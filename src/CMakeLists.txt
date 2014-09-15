include_directories(${CMAKE_SOURCE_DIR}/include
                    ${ROOT_INCLUDE_DIRS}
)

file(GLOB sources *.cc)

add_library(albers SHARED ${sources}) 
target_link_libraries(albers ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/albers DESTINATION include)
install(TARGETS albers DESTINATION lib)
