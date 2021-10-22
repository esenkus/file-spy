cmake_minimum_required(VERSION 3.20)
project(file_spy)

set(CMAKE_CXX_STANDARD 14)

enable_testing()

add_subdirectory(main)
add_subdirectory(file-spy-test)
add_subdirectory(file-spy-lib)
