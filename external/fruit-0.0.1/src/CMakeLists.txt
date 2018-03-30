###################################################################################
# add headers files
###################################################################################
file(GLOB INCLUDE "../include/*.h")
source_group(include FILES ${INCLUDE})

file(GLOB FRUIT_CORE "../include/fruit_core/*.h")
source_group(include\\fruit_core			FILES ${FRUIT_CORE})

file(GLOB FRUIT_EXTENSION "../include/fruit_extensions/*.h")
source_group(include\\fruit_extension	FILES ${FRUIT_EXTENSION})		

file(GLOB FRUIT "../include/fruit/*.h")
source_group(include\\fruit FILES ${FRUIT})

file(GLOB HEADERS_UTILITS "../include/fruit/utilits/*.h")
source_group(include\\fruit\\utilits FILES ${HEADERS_UTILITS})

file(GLOB HEADERS_CONTROLLER "../include/fruit/controller/*.h")
source_group(include\\fruit\\controller FILES ${HEADERS_CONTROLLER})

file(GLOB HEADERS_COMPUTE_CONTROLLER "../include/fruit/controller/compute_controller/*.h")
source_group(include\\fruit\\controller\\compute_controller FILES ${HEADERS_COMPUTE_CONTROLLER})

file(GLOB HEADERS_IMGUI_CONTROLLER "../include/fruit/controller/imgui_controller/*.h")
source_group(include\\fruit\\controller\\imgui_controller FILES ${HEADERS_IMGUI_CONTROLLER})

file(GLOB HEADERS_RENDER_CONTROLLER	"../include/fruit/controller/render_controller/*.h")
source_group(include\\fruit\\controller\\render_controller	FILES ${HEADERS_RENDER_CONTROLLER})

file(GLOB HEADERS_FLUID "../include/fruit/controller/render_controller/fluid/*.h")
source_group(include\\fruit\\controller\\render_controller\\fluid	FILES ${HEADERS_FLUID})

file(GLOB HEADERS_RENDER_GL "../include/fruit/controller/render_controller/render_gl/*.h")
source_group(include\\fruit\\controller\\render_controller\\render_gl	FILES ${HEADERS_RENDER_GL})

###################################################################################
# add source files
###################################################################################
set(SRC application.cpp)
source_group(src FILES ${SRC})

file(GLOB UTILITS "utilits/*.h" "utilits/*.cpp")
source_group(src\\utilits FILES ${UTILITS})

# add source controllers
file(GLOB CONTROLLER "controller/*.cpp" "controller/*.h")
source_group(src\\controller FILES ${CONTROLLER})

file(GLOB COMPUTE_CONTROLLER "controller/compute_controller/*.cpp" "controller/compute_controller/*.h")
source_group(src\\controller\\compute_controller FILES ${COMPUTE_CONTROLLER})

file(GLOB IMGUI_CONTROLLER "controller/imgui_controller/*.cpp" "controller/imgui_controller/*.h")
source_group(src\\controller\\imgui_controller FILES ${IMGUI_CONTROLLER})

file(GLOB RENDER_CONTROLLER	"controller/render_controller/*.cpp" "controller/render_controller/*.h")
source_group(src\\controller\\render_controller			FILES ${RENDER_CONTROLLER})

file(GLOB FLUID "controller/render_controller/Fluid/*.cpp" "controller/render_controller/Fluid/*.h")
source_group(src\\controller\\render_controller\\fluid	FILES ${FLUID})

file(GLOB RENDER_GL "controller/render_controller/render_gl/*.cpp" "controller/render_controller/render_gl/*.h")
source_group(src\\controller\\render_controller\\render_gl	FILES ${RENDER_GL})

###################################################################################

add_library(FruitWork STATIC  
								${INCLUDE} 
									${FRUIT}
										${HEADERS_UTILITS}
										${HEADERS_CONTROLLER}
											${HEADERS_COMPUTE_CONTROLLER}
											${HEADERS_IMGUI_CONTROLLER}
											${HEADERS_RENDER_CONTROLLER}
												${HEADERS_FLUID}
												${HEADERS_RENDER_GL}
									${FRUIT_CORE}
									${FRUIT_EXTENSION} 
								${SRC}
									${CONTROLLER}
										${COMPUTE_CONTROLLER}
										${IMGUI_CONTROLLER}
										${RENDER_CONTROLLER}
											${FLUID}
											${RENDER_GL}
									${UTILITS}
)
								