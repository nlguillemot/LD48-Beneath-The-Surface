cmake_minimum_required(VERSION 2.8.3)
project(game CXX)

find_package(SDL2plus REQUIRED)
find_package(GLmesh REQUIRED)
find_package(glm REQUIRED)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(SOURCES
    main.cpp
    gamecontext.hpp gamecontext.cpp
    rendercontext.hpp
    scene.hpp
    worldscene.hpp worldscene.cpp
    billboard.hpp billboard.cpp
    geometry.hpp geometry.cpp
    debugdraw.hpp debugdraw.cpp)

include_directories(
    ${SDL2plus_INCLUDE_DIRS}
    ${GLmesh_INCLUDE_DIRS}
    ${glm_INCLUDE_DIR})

add_executable(game ${SOURCES})

target_link_libraries(game
    ${SDL2plus_LIBRARIES}
    ${GLmesh_LIBRARIES})

# temporary. can be removed when glm 0.9.6 comes out.
add_definitions(-DGLM_FORCE_RADIANS)

set(ASSETS
    floor.obj floor.mtl floor.png
    player.png
     mound.png mound0.png mound1.png mound2.png mound3.png
    mound4.png mound5.png mound6.png mound7.png mound8.png
    mine.png
    world.vs world.fs
    debug.vs debug.fs)

foreach(assetFile ${ASSETS})
    add_custom_target(copy_${assetFile} ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${assetFile}
            ${CMAKE_CURRENT_BINARY_DIR}/${assetFile}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${assetFile})
endforeach()
