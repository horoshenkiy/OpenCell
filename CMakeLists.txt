cmake_minimum_required(VERSION 3.8)
project(OpenCell LANGUAGES CXX CUDA)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../../bin/")

add_subdirectory(src)
add_subdirectory(extensions)
