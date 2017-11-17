cmake_minimum_required(VERSION 3.8)
project(OpenCell LANGUAGES CXX CUDA)

# main project

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../../bin/")

include_directories(core)
include_directories(fruit_core fruit_extensions)
include_directories(external/cereal/include)	

add_subdirectory(src)
add_subdirectory(extensions)

# test 

include_directories(external/googletest/include)
include_directories(src)

link_directories(external/googletest/lib/Win64)

add_subdirectory(test)
