cmake_minimum_required(VERSION 3.8)

include("cmake/HunterGate.cmake")

HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.20.24.tar.gz"
    SHA1 "3e2037a462bcf2ec3440f60298d933e74ffd4df3"
)


project(OpenCell LANGUAGES CXX CUDA)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#########################################################################
# Flex project (DLL)
#########################################################################

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

add_subdirectory(external/flex-1.1.0/extension)
add_subdirectory(external/flex-1.1.0/core)

#########################################################################
# fruit project (Static lib)
#########################################################################

include_directories(external/cereal/include)	
include_directories(external/cxxopts-1.4.3/include)

include_directories(external/flex-1.1.0/core/include external/flex-1.1.0/core/include/flex/core)
include_directories(external/flex-1.1.0/include)
include_directories(external/flex-1.1.0/extension/include)

include_directories(external/SDL2-2.0.4/include)
include_directories(external/glew/include)

link_directories(external/flex-1.1.0/lib/win64)
link_directories(external/SDL2-2.0.4/lib/x64)
link_directories(external/glew/lib/x64)

link_directories(${CMAKE_BINARY_DIR}/lib/Release ${CMAKE_BINARY_DIR}/lib/Debug)

# add boost
hunter_add_package(Boost)
find_package(Boost)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  message( STATUS "Add boost headers")
endif()

# add source
add_subdirectory(external/fruit-0.0.1)

#########################################################################
# open_cell project
#########################################################################

include_directories(external/fruit-0.0.1/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/)

add_subdirectory(src)
add_subdirectory(test)
