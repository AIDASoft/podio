cmake_minimum_required(VERSION 2.8)

project(albers)

set(CPP11FLAGS "-std=c++11")
if (${APPLE})
    set(CPP11FLAGS "-std=c++11\ -stdlib=libc++")
endif()
message(${CPP11FLAGS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -DDROP_CGAL ${CPP11FLAGS} -Wall -Wextra -Wpedantic -Wno-unused-variable -Wno-unused-parameter") # -Wpadded
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Declare ROOT dependency
find_package(ROOT COMPONENTS Core RIO Tree)

include_directories(${ROOT_INCLUDE_DIR})
#include(${ROOT_USE_FILE})

add_subdirectory(src)
add_subdirectory(examples)
