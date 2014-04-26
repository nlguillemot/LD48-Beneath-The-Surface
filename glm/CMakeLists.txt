cmake_minimum_required(VERSION 2.8.3)
project(glm)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

add_subdirectory(include/glm)

# note: SOURCES inherited from include/glm/CMakeLists.txt
add_library(glm STATIC dummy.cpp ${SOURCES})
