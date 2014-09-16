cmake_minimum_required(VERSION 2.8)

project(albers)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -DDROP_CGAL -std=c++11 -Wall -Wextra -Wpedantic -Wno-unused-variable -Wno-unused-parameter")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

# Declare ROOT dependency
find_package(ROOT COMPONENTS EG Eve GenVector Hist Physics Matrix Graf RIO Tree Gpad RGL MathCore)
include(${ROOT_USE_FILE})

add_subdirectory(src)
add_subdirectory(examples)
