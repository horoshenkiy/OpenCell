#pragma once

#include "../../../opengl/shader.h"
#include "../../../../include/NvFlex.h"

#include "../../compute_controller/FlexController.h"
extern FlexController flexController;

typedef unsigned int VertexBuffer;
typedef unsigned int IndexBuffer;

struct FluidRenderBuffers
{
	VertexBuffer mPositionVBO;
	VertexBuffer mDensityVBO;
	VertexBuffer mAnisotropyVBO[3];
	IndexBuffer mIndices;

	VertexBuffer mFluidVBO; // to be removed

							// wrapper buffers that allow Flex to write directly to VBOs
	NvFlexBuffer* mPositionBuf;
	NvFlexBuffer* mDensitiesBuf;
	NvFlexBuffer* mAnisotropyBuf[3];
	NvFlexBuffer* mIndicesBuf;

	int mNumFluidParticles;
};

FluidRenderBuffers CreateFluidRenderBuffers(int numParticles, bool enableInterop);

void DestroyFluidRenderBuffers(FluidRenderBuffers buffers);

void UpdateFluidRenderBuffers(FluidRenderBuffers buffers, NvFlexSolver* solver, bool anisotropy, bool density);

void UpdateFluidRenderBuffers(FluidRenderBuffers buffers, 
							  Vec4* particles, float* densities, 
							  Vec4* anisotropy1, Vec4* anisotropy2, 
							  Vec4* anisotropy3, int numParticles, 
							  int* indices, int numIndices);