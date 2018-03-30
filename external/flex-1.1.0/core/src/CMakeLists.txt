add_library(NvFlexCore STATIC 
								aabbtree.cpp
								core.cpp
								extrude.cpp
								maths.cpp
								mesh.cpp
								perlin.cpp
								pfm.cpp
								platform.cpp
								sdf.cpp
								tga.cpp
								voxelize.cpp
)

set_target_properties(NvFlexCore PROPERTIES OUTPUT_NAME_DEBUG NvFlexCoreDebug_x64)
set_target_properties(NvFlexCore PROPERTIES OUTPUT_NAME_RELEASE NvFlexCoreRelease_x64)

