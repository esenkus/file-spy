cmake_minimum_required(VERSION 3.20)
project(file_spy)

set(CMAKE_CXX_STANDARD 14)

add_executable(file_spy main.cpp domain/Observer.cpp domain/Observer.hpp)
