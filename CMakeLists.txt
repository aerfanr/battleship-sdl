cmake_minimum_required(VERSION 3.10)

project(battleship CXX)

set(SOURCES
	src/main.cpp
	src/draw.h src/draw.cpp
)

add_executable(battleship ${SOURCES})

target_include_directories(battleship PRIVATE src)

target_link_libraries(battleship SDL2main SDL2)
target_link_libraries(battleship SDL2_image SDL2)
