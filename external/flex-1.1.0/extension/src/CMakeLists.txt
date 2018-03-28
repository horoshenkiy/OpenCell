set(SOURCE_CORE
	../../core/src/aabbtree.cpp
	../../core/src/maths.cpp
	../../core/src/sdf.cpp
	../../core/src/voxelize.cpp
)

set(SOURCE_EXT_CUDA
	cuda/flexExt.cu
)

set(SOURCE_EXT
	flexExtCloth.cpp
	flexExtContainer.cpp
	flexExtMovingFrame.cpp
	flexExtRigid.cpp
	flexExtSoft.cpp
	cuda/flexExt.cu
)

add_library(NvFlexExtCUDA SHARED ${SOURCE_EXT} ${SOURCE_EXT_CUDA} ${SOURCE_CORE})
target_link_libraries(NvFlexExtCUDA optimized NvFlexReleaseCUDA_x64)
target_link_libraries(NvFlexExtCUDA debug NvFlexDebugCUDA_x64)

set_target_properties(NvFlexExtCUDA PROPERTIES OUTPUT_NAME_DEBUG NvFlexExtDebugCUDA_x64)
set_target_properties(NvFlexExtCUDA PROPERTIES OUTPUT_NAME_RELEASE NvFlexExtReleaseCUDA_x64)

source_group("src" FILES ${SOURCE_EXT})
source_group("cuda" FILES ${SOURCE_EXT_CUDA})
source_group("core" FILES ${SOURCE_CORE})