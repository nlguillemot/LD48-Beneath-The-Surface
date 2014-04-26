cmake_minimum_required(VERSION 2.8.3)
project(SDL2plus CXX)

include(cmake/FindSDL2plus.cmake)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

include_directories(${SDL2plus_INCLUDE_DIRS})

add_library(${SDL2plus_LIBRARY}
    include/SDL2plus.hpp
    src/SDL2plus.cpp)

target_link_libraries(${SDL2plus_LIBRARY} ${SDL2plus_DEPENDENCIES})
