cmake_minimum_required(VERSION 2.8.3)

# quick macro to simultaneously add subdirectories
#     and add their FindXXX.cmake files to the CMAKE_MODULE_PATH.
macro(add_subproject subproject)
    add_subdirectory(${subproject})
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/${subproject}/cmake")
endmacro()

add_subproject(glm)
add_subproject(glew)
add_subproject(soil2)
add_subproject(tinyobjloader)
add_subproject(GLplus)
add_subproject(GLmesh)
add_subproject(SDL2)
add_subproject(SDL2plus)
add_subproject(game)

