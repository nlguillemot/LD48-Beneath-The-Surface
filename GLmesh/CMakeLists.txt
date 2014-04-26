cmake_minimum_required(VERSION 2.6)
project(GLmesh CXX)

include(cmake/FindGLmesh.cmake)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

include_directories(${GLmesh_INCLUDE_DIRS})

add_library(${GLmesh_LIBRARY}
    include/GLmesh.hpp
    src/GLmesh.cpp)

target_link_libraries(${GLmesh_LIBRARY} ${GLmesh_DEPENDENCIES})
