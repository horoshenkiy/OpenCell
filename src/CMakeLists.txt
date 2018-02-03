link_directories("../lib/win64")
link_directories("../external/SDL2-2.0.4/lib/x64")
link_directories("../external/glew/lib/x64")

file(GLOB CORE "../core/*.h" "../core/*.cpp")

file(GLOB SRC			"./*.cpp" "./*.h")

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

file(GLOB FRUIT_CORE	  "../fruit_core/*.h")
file(GLOB FRUIT_EXTENSION "../fruit_extensions/*.h")
file(GLOB INCLUDE 		  "../include/*.h")
file(GLOB OPEN_GL 		  "opengl/*.cpp" "opengl/*.h")

add_executable(OpenCell main.cpp 	${CORE}
									${SRC}
										${SCENES}
											${COMPONENTS}
										${CONTROLLER}
											${COMPUTE_CONTROLLER}
											${IMGUI_CONTROLLER}
											${RENDER_CONTROLLER}
												${FLUID}
									${FRUIT_CORE}
									${FRUIT_EXTENSION} 
									${INCLUDE} 
									${OPEN_GL})
									
target_link_libraries(OpenCell NvFlexReleaseCUDA_x64)
target_link_libraries(OpenCell NvFlexDeviceRelease_x64)
target_link_libraries(OpenCell NvFlexExtReleaseCUDA_x64)
target_link_libraries(OpenCell SDL2)
target_link_libraries(OpenCell SDL2main)
target_link_libraries(OpenCell glew32)

source_group(core FILES ${CORE}) 

source_group(src						FILES ${SRC})

source_group(src\\scenes 				FILES ${SCENES})
source_group(src\\scenes\\components 	FILES ${COMPONENTS})

source_group(src\\controller							FILES ${CONTROLLER})
source_group(src\\controller\\compute_controller 		FILES ${COMPUTE_CONTROLLER})
source_group(src\\controller\\imgui_controller			FILES ${IMGUI_CONTROLLER})
source_group(src\\controller\\render_controller			FILES ${RENDER_CONTROLLER})
source_group(src\\controller\\render_controller\\Fluid	FILES ${FLUID})
	
source_group(fruit_core			FILES ${FRUIT_CORE})
source_group(fruit_extension	FILES ${FRUIT_EXTENSION})							 
source_group(include  			FILES ${INCLUDE})
source_group(opengl 			FILES ${OPEN_GL})