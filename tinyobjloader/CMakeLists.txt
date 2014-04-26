cmake_minimum_required(VERSION 2.8.3)
project(tinyobjloader CXX)

include(cmake/Findtinyobjloader.cmake)

include_directories(${tinyobjloader_INCLUDE_DIR})

set(SOURCES
    include/tiny_obj_loader.h
    src/tiny_obj_loader.cc)

add_library(${tinyobjloader_LIBRARY} STATIC ${SOURCES})
