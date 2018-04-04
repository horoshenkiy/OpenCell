file(GLOB INCLUDE "../include/*.h")
source_group(include FILES ${INCLUDE})

file(GLOB INCLUDE_COMPONENTS "../include/components/*.h")
source_group(include\\components FILES ${INCLUDE_COMPONENTS})

file(GLOB SRC "*.h" "*.cpp")
source_group(src FILES ${SRC})

file(GLOB COMPONENTS "components/*.h" "components/*.cpp")
source_group(src\\components FILES ${COMPONENTS})

add_executable(OpenCell ${INCLUDE} ${INCLUDE_COMPONENTS} ${SRC} ${COMPONENTS})

target_link_libraries(OpenCell optimized NvFlexReleaseCUDA_x64)
target_link_libraries(OpenCell optimized NvFlexDeviceRelease_x64)
target_link_libraries(OpenCell optimized NvFlexExtReleaseCUDA_x64) 
target_link_libraries(OpenCell optimized NvFlexCoreRelease_x64) 
target_link_libraries(OpenCell optimized FruitWork) 
	
target_link_libraries(OpenCell debug NvFlexDebugCUDA_x64)
target_link_libraries(OpenCell debug NvFlexDeviceDebug_x64)
target_link_libraries(OpenCell debug NvFlexExtDebugCUDA_x64d.lib)
target_link_libraries(OpenCell debug NvFlexCoreDebug_x64d.lib) 
target_link_libraries(OpenCell debug FruitWorkd) 

target_link_libraries(OpenCell SDL2 SDL2main glew32) 