# Defines:
# glew_FOUND - Always true
# glew_INCLUDE_DIR - Only the glew include directory
# glew_INCLUDE_DIRS - All include directories needed by glew
# glew_LIBRARY - Only the glew library
# glew_DEPENDENCIES - All libraries needed by glew
# glew_LIBRARIES - All libraries needed by glew and glew

find_package(OpenGL REQUIRED)

set(glew_FOUND TRUE)
set(glew_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../include)
set(glew_INCLUDE_DIRS ${glew_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
set(glew_LIBRARY glew)
set(glew_DEPENDENCIES ${OPENGL_LIBRARIES})
set(glew_LIBRARIES ${glew_LIBRARY} ${glew_DEPENDENCIES})
