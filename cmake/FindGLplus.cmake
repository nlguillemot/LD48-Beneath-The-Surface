# Defines:
# GLplus_FOUND - Always true
# GLplus_INCLUDE_DIR - Only the include directory for GLplus
# GLplus_INCLUDE_DIRS - All include directories needed by GLplus
# GLplus_LIBRARY - The GLplus library only
# GLplus_DEPENDENCIES - All libraries that GLplus depends on
# GLplus_LIBRARIES - GLplus and all its needed libraries

find_package(OpenGL REQUIRED)
find_package(glew REQUIRED)
find_package(soil2 REQUIRED)

set(GLplus_FOUND TRUE)
set(GLplus_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../include)
set(GLplus_INCLUDE_DIRS
    ${GLplus_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${glew_INCLUDE_DIR}
    ${soil2_INCLUDE_DIR})
set(GLplus_LIBRARY GLplus)
set(GLplus_DEPENDENCIES
    ${OPENGL_LIBRARIES}
    ${glew_LIBRARIES}
    ${soil2_LIBRARY})
set(GLplus_LIBRARIES ${GLplus_LIBRARY} ${GLplus_DEPENDENCIES})
