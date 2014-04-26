cmake_minimum_required(VERSION 2.8.3)
project(GLplus CXX)

include(cmake/FindGLplus.cmake)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

INCLUDE_DIRECTORIES(${GLplus_INCLUDE_DIRS})

ADD_LIBRARY(${GLplus_LIBRARY}
    include/GLplus.hpp
    src/GLplus.cpp)

TARGET_LINK_LIBRARIES(${GLplus_LIBRARY} ${GLplus_DEPENDENCIES})
