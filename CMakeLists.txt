cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("Terminal games")

file(GLOB_RECURSE SRCS src/*.cpp)

add_executable(tetris ${SRCS})
