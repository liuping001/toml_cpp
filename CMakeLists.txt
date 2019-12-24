cmake_minimum_required(VERSION 3.2)
project(toml_cpp)
set(CMAKE_CXX_STANDARD 11)

include_directories(.)
include_directories(third)

add_executable(toml_cpp toml_make_cpp.cpp)
add_executable( test test/test_toml.cpp)
