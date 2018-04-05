file(GLOB INCLUDE "../include/*.h")
source_group(include FILES ${INCLUDE})

file(GLOB INCLUDE_COMPONENTS "../include/components/*.h")
source_group(include\\components FILES ${INCLUDE_COMPONENTS})

file(GLOB SRC "*.h" "*.cpp")
source_group(src FILES ${SRC})

file(GLOB COMPONENTS "components/*.h" "components/*.cpp")
source_group(src\\components FILES ${COMPONENTS})

add_executable(OpenCell ${INCLUDE} ${INCLUDE_COMPONENTS} ${SRC} ${COMPONENTS})
target_link_libraries(OpenCell FruitWork) 
