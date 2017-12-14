cmake_minimum_required(VERSION 3.9)
project(clion)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-fcilkplus -fopenmp -std=c++11")

add_executable(clion main.cpp)
target_compile_options(clion PUBLIC -fcilkplus)
set(CMAKE_VERBOSE_MAKEFILE ON)