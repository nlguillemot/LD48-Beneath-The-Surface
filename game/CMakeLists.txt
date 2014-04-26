cmake_minimum_required(VERSION 2.8.3)
project(game CXX)

find_package(SDL2plus REQUIRED)
find_package(GLmesh REQUIRED)
find_package(glm REQUIRED)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(SOURCES
    main.cpp)

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

set(ASSETS "")

foreach(assetFile ${ASSETS})
	configure_file(${assetFile} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endforeach()
