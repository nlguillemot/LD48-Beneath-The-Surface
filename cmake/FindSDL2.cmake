# Defines:
# SDL2_FOUND - Always true
# SDL2_INCLUDE_DIR - The include directory of SDL2
# SDL2_LIBRARIES - The libraries to link for SDL2

set(SDL2_FOUND TRUE)
set(SDL2_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../include)
set(SDL2_LIBRARIES SDL2-static SDL2main)
