#
# Author: Kevin XU <kevin.xu.1982.02.06@gmail.com>
#

cmake_minimum_required(VERSION 3.0)

project(APRCS LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -std=c++17 -pedantic-errors -Wall")

add_library(APRCS OBJECT ".compile.cpp")

enable_testing()

add_subdirectory("test")

add_subdirectory("samples")

