file(GLOB SRC "*.h" "*.cpp")
source_group(src FILES ${SRC})

file(GLOB COMPONENTS "components/*.h" "components/*.cpp")
source_group(src\\components FILES ${COMPONENTS})

add_executable(OpenCell ${SRC} ${COMPONENTS})

target_link_libraries(OpenCell optimized NvFlexReleaseCUDA_x64)
target_link_libraries(OpenCell optimized NvFlexDeviceRelease_x64)
target_link_libraries(OpenCell optimized NvFlexExtReleaseCUDA_x64) 
target_link_libraries(OpenCell optimized NvFlexCoreRelease_x64.lib) 
target_link_libraries(OpenCell optimized FruitWork.lib) 
	
target_link_libraries(OpenCell debug NvFlexDebugCUDA_x64)
target_link_libraries(OpenCell debug NvFlexDeviceDebug_x64)
target_link_libraries(OpenCell debug NvFlexExtDebugCUDA_x64)
target_link_libraries(OpenCell debug NvFlexCoreDebug_x64.lib) 
target_link_libraries(OpenCell debug FruitWorkd.lib) 

target_link_libraries(OpenCell SDL2 SDL2main glew32) 