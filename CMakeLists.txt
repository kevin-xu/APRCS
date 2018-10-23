
cmake_minimum_required(VERSION 3.0)

project(APRCS CXX)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -std=c++11 -pedantic-errors -Wall")

include_directories("include")

add_library(APRCS OBJECT ".compile.cpp")

enable_testing()

add_subdirectory("test")

add_subdirectory("samples")

