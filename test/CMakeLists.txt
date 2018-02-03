include_directories(../core ../fruit_core ../fruit_extensions ../external/cereal/include ../external/googletest/include)

link_directories(../lib/win64 
				 ../bin/Debug 
				 ../bin/Release 
				 ../external/googletest/lib/Win64
				 ../external/SDL2-2.0.4/lib/x64
				 ../external/glew/lib/x64)

set(CompilerFlags
		CMAKE_CXX_FLAGS
		CMAKE_CXX_FLAGS_DEBUG
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_C_FLAGS
		CMAKE_C_FLAGS_DEBUG
		CMAKE_C_FLAGS_RELEASE
)

foreach(CompilerFlag ${CompilerFlags})
	string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
	string(REPLACE "/MTd" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()

#add source  
###############################################

file(GLOB CORE "../core/*.h" "../core/*.cpp")

set(SRC ../src/Utilits.cpp)
set(OPENGL ../src/opengl/shader.cpp ../src/opengl/shadersGL.cpp)

set(COMPONENT ../src/scenes/cell.cpp
			  ../src/scenes/SceneCell.cpp
			  ../src/scenes/components/Branch.cpp
			  ../src/scenes/components/Cytoplasm.cpp
			  ../src/scenes/components/Cytoskeleton.cpp
			  ../src/scenes/components/Kernel.cpp
			  ../src/scenes/components/Shell.cpp)

set(COMPUTE_CONTROLLER ../src/controller/compute_controller/FlexController.cpp
					   ../src/controller/compute_controller/FlexParams.cpp
					   ../src/controller/compute_controller/SimBuffers.cpp)
					   
set(RENDER_CONTROLLER ../src/controller/render_controller/Fluid/FluidBuffer.cpp
					  ../src/controller/render_controller/Fluid/FluidRenderer.cpp
					  ../src/controller/render_controller/RenderController.cpp
					  ../src/controller/render_controller/Shadows.cpp)
					
###############################################
				
add_executable(OpenCellTest 
							main.cpp 			
							../src/controller/compute_controller/FlexController.cpp
							${CORE} ${SRC} ${OPENGL} ${COMPONENT} ${COMPUTE_CONTROLLER} ${RENDER_CONTROLLER}
)

#link libraries					
###############################################

target_link_libraries(OpenCellTest glew32.lib SDL2.lib SDL2main.lib gtest.lib)
							
target_link_libraries(OpenCellTest optimized NvFlexReleaseCUDA_x64)
target_link_libraries(OpenCellTest optimized NvFlexDeviceRelease_x64)
target_link_libraries(OpenCellTest optimized NvFlexExtReleaseCUDA_x64)
	
target_link_libraries(OpenCellTest debug NvFlexDebugCUDA_x64)
target_link_libraries(OpenCellTest debug NvFlexDeviceDebug_x64)
target_link_libraries(OpenCellTest debug NvFlexExtDebugCUDA_x64)