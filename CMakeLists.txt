cmake_minimum_required(VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("kilo")

add_executable(kilo "kilo.cpp" "tetris.cpp" "snake.cpp" "render.cpp")
