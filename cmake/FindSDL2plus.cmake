# Defines:
# SDL2plus_FOUND - Always true
# SDL2plus_LIBRARY - The library for SDL2plus
# SDL2plus_DEPENDENCIES - The libraries SDL2plus depends on
# SDL2plus_LIBRARIES - The library for SDL2plus and all dependencies
# SDL2plus_INCLUDE_DIR - The include directory for SDL2plus
# SDL2plus_INCLUDE_DIRS - All include directories needed by SDL2plus

find_package(glew REQUIRED)
find_package(SDL2 REQUIRED)

set(SDL2plus_FOUND TRUE)
set(SDL2plus_LIBRARY SDL2plus)
set(SDL2plus_DEPENDENCIES ${glew_LIBRARIES} ${SDL2_LIBRARIES})
set(SDL2plus_LIBRARIES ${SDL2plus_LIBRARY} ${SDL2plus_DEPENDENCIES})
set(SDL2plus_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../include)
set(SDL2plus_INCLUDE_DIRS
    ${SDL2plus_INCLUDE_DIR}
    ${glew_INCLUDE_DIRS}
	${SDL2_INCLUDE_DIR})
