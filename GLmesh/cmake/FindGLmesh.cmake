# Defines:
# GLmesh_FOUND - Always true
# GLmesh_LIBRARY - The library name for GLmesh
# GLmesh_DEPENDENCIES - All libraries GLmesh depends on
# GLmesh_LIBRARIES - GLmesh and all libraries it depends on

find_package(tinyobjloader REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLplus REQUIRED)
find_package(glew REQUIRED)

set(GLmesh_FOUND TRUE)
set(GLmesh_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../include)
set(GLmesh_INCLUDE_DIRS
	${GLmesh_INCLUDE_DIR}
    ${tinyobjloader_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIRS}
    ${GLplus_INCLUDE_DIRS}
    ${glew_INCLUDE_DIRS})
set(GLmesh_LIBRARY GLmesh)
set(GLmesh_DEPENDENCIES
    ${tinyobjloader_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${GLplus_LIBRARIES}
    ${glew_LIBRARIES})
set(GLmesh_LIBRARIES ${GLmesh_LIBRARY} ${GLmesh_DEPENDENCIES})
