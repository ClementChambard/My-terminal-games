cmake_minimum_required(VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED)
set(CMAKE_CXX_COMPILER "g++")

project("kilo")

add_executable(kilo "kilo.cpp" "tetris.cpp" "snake.cpp" "render.cpp")
