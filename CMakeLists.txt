cmake_minimum_required(VERSION 2.8)

project(albers)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -DDROP_CGAL -std=c++11 -stdlib=libc++ -Wall -Wextra -Wpedantic -Wno-unused-variable -Wno-unused-parameter")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Declare ROOT dependency
find_package(ROOT)

include_directories(${ROOT_INCLUDE_DIR})
# include(${ROOT_USE_FILE})

add_subdirectory(src)
add_subdirectory(examples)
