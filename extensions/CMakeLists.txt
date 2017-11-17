set(SOURCE_CORE
	../core/aabbtree.cpp
	../core/maths.cpp
	../core/sdf.cpp
	../core/voxelize.cpp
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

link_directories("../lib/win64")

add_library(flexExtCUDA SHARED ${SOURCE_EXT} ${SOURCE_EXT_CUDA} ${SOURCE_CORE})
target_link_libraries(flexExtCUDA optimized NvFlexReleaseCUDA_x64)
target_link_libraries(flexExtCUDA debug NvFlexDebugCUDA_x64)

set_target_properties(flexExtCUDA PROPERTIES OUTPUT_NAME_DEBUG NvFlexExtDebugCUDA_x64)
set_target_properties(flexExtCUDA PROPERTIES OUTPUT_NAME_RELEASE NvFlexExtReleaseCUDA_x64)

source_group("src" FILES ${SOURCE_EXT})
source_group("cuda" FILES ${SOURCE_EXT_CUDA})
source_group("core" FILES ${SOURCE_CORE})