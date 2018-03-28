file(GLOB CORE "../external/flex-1.1.0/core/include/flex/core/*.h" 
			   "../external/flex-1.1.0/core/src/*.cpp")

file(GLOB SRC "./*.cpp" "./*.h")

file(GLOB SCENES		"scenes/*.cpp" "scenes/*.h")
file(GLOB COMPONENTS 	"scenes/components/*.cpp" "scenes/components/*.h")

file(GLOB CONTROLLER 	  	 	"controller/*.cpp" "controller/*.h")
file(GLOB COMPUTE_CONTROLLER 	"controller/compute_controller/*.cpp" 
								"controller/compute_controller/*.h")
file(GLOB IMGUI_CONTROLLER		"controller/imgui_controller/*.cpp"
								"controller/imgui_controller/*.h")
file(GLOB RENDER_CONTROLLER		"controller/render_controller/*.cpp"
								"controller/render_controller/*.h")
file(GLOB FLUID 				"controller/render_controller/Fluid/*.cpp"
								"controller/render_controller/Fluid/*.h")

file(GLOB FRUIT_CORE	  "../include/fruit_core/*.h")
file(GLOB FRUIT_EXTENSION "../include/fruit_extensions/*.h")
file(GLOB INCLUDE 		  "../include/*.h")
file(GLOB OPEN_GL 		  "opengl/*.cpp" "opengl/*.h")
file(GLOB UTILITS		  "utilits/*.h" "utilits/*.cpp")

add_executable(OpenCell main.cpp 
									${INCLUDE} 
									${SRC}
										${SCENES}
											${COMPONENTS}
										${CONTROLLER}
											${COMPUTE_CONTROLLER}
											${IMGUI_CONTROLLER}
											${RENDER_CONTROLLER}
												${FLUID}
										${UTILITS}
									${CORE}
									${FRUIT_CORE}
									${FRUIT_EXTENSION} 
									${OPEN_GL})
									
target_link_libraries(OpenCell optimized NvFlexReleaseCUDA_x64)
target_link_libraries(OpenCell optimized NvFlexDeviceRelease_x64)
target_link_libraries(OpenCell optimized NvFlexExtReleaseCUDA_x64.lib)
	
target_link_libraries(OpenCell debug NvFlexDebugCUDA_x64)
target_link_libraries(OpenCell debug NvFlexDeviceDebug_x64)
target_link_libraries(OpenCell debug NvFlexExtDebugCUDA_x64)

target_link_libraries(OpenCell SDL2 SDL2main glew32)

source_group(core FILES ${CORE}) 

source_group(src						FILES ${SRC})

source_group(src\\scenes 				FILES ${SCENES})
source_group(src\\scenes\\components 	FILES ${COMPONENTS})

source_group(src\\utilits 				FILES ${UTILITS})

source_group(src\\controller							FILES ${CONTROLLER})
source_group(src\\controller\\compute_controller 		FILES ${COMPUTE_CONTROLLER})
source_group(src\\controller\\imgui_controller			FILES ${IMGUI_CONTROLLER})
source_group(src\\controller\\render_controller			FILES ${RENDER_CONTROLLER})
source_group(src\\controller\\render_controller\\Fluid	FILES ${FLUID})
	
source_group(fruit_core			FILES ${FRUIT_CORE})
source_group(fruit_extension	FILES ${FRUIT_EXTENSION})							 
source_group(include  			FILES ${INCLUDE})
source_group(src\\opengl 			FILES ${OPEN_GL})