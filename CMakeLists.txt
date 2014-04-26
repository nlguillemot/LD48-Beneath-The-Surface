cmake_minimum_required(VERSION 2.8.3)
project(glew C)

include(cmake/Findglew.cmake)

include_directories(${glew_INCLUDE_DIRS})

add_library(${glew_LIBRARY} STATIC
    include/GL/glew.h
    include/GL/glxew.h
    include/GL/wglew.h
    src/glew.c)

target_link_libraries(${glew_LIBRARY} ${glew_DEPENDENCIES})
