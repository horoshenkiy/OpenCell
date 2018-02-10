#include "FluidBuffer.h"

FluidRenderBuffers CreateFluidRenderBuffers(int numFluidParticles, bool enableInterop)
{
	FluidRenderBuffers buffers = {};
	buffers.mNumFluidParticles = numFluidParticles;

	// vbos
	glVerify(glGenBuffers(1, &buffers.mPositionVBO));
	glVerify(glBindBuffer(GL_ARRAY_BUFFER, buffers.mPositionVBO));
	glVerify(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * numFluidParticles, 0, GL_DYNAMIC_DRAW));

	// density
	glVerify(glGenBuffers(1, &buffers.mDensityVBO));
	glVerify(glBindBuffer(GL_ARRAY_BUFFER, buffers.mDensityVBO));
	glVerify(glBufferData(GL_ARRAY_BUFFER, sizeof(int)*numFluidParticles, 0, GL_DYNAMIC_DRAW));

	for (int i = 0; i < 3; ++i)
	{
		glVerify(glGenBuffers(1, &buffers.mAnisotropyVBO[i]));
		glVerify(glBindBuffer(GL_ARRAY_BUFFER, buffers.mAnisotropyVBO[i]));
		glVerify(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * numFluidParticles, 0, GL_DYNAMIC_DRAW));
	}

	glVerify(glGenBuffers(1, &buffers.mIndices));
	glVerify(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.mIndices));
	glVerify(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*numFluidParticles, 0, GL_DYNAMIC_DRAW));

	FlexController &flexController = FlexController::Instance();

	// delete it
	if (enableInterop)
	{
		buffers.mPositionBuf = NvFlexRegisterOGLBuffer(flexController.GetLib(), buffers.mPositionVBO, numFluidParticles, sizeof(Vec4));
		buffers.mDensitiesBuf = NvFlexRegisterOGLBuffer(flexController.GetLib(), buffers.mDensityVBO, numFluidParticles, sizeof(float));
		buffers.mIndicesBuf = NvFlexRegisterOGLBuffer(flexController.GetLib(), buffers.mIndices, numFluidParticles, sizeof(int));

		buffers.mAnisotropyBuf[0] = NvFlexRegisterOGLBuffer(flexController.GetLib(), buffers.mAnisotropyVBO[0], numFluidParticles, sizeof(Vec4));
		buffers.mAnisotropyBuf[1] = NvFlexRegisterOGLBuffer(flexController.GetLib(), buffers.mAnisotropyVBO[1], numFluidParticles, sizeof(Vec4));
		buffers.mAnisotropyBuf[2] = NvFlexRegisterOGLBuffer(flexController.GetLib(), buffers.mAnisotropyVBO[2], numFluidParticles, sizeof(Vec4));
	}

	return buffers;
}

void DestroyFluidRenderBuffers(FluidRenderBuffers buffers)
{
	glDeleteBuffers(1, &buffers.mPositionVBO);
	glDeleteBuffers(3, buffers.mAnisotropyVBO);
	glDeleteBuffers(1, &buffers.mDensityVBO);
	glDeleteBuffers(1, &buffers.mIndices);

	NvFlexUnregisterOGLBuffer(buffers.mPositionBuf);
	NvFlexUnregisterOGLBuffer(buffers.mDensitiesBuf);
	NvFlexUnregisterOGLBuffer(buffers.mIndicesBuf);

	NvFlexUnregisterOGLBuffer(buffers.mAnisotropyBuf[0]);
	NvFlexUnregisterOGLBuffer(buffers.mAnisotropyBuf[1]);
	NvFlexUnregisterOGLBuffer(buffers.mAnisotropyBuf[2]);
}

// download anisotropy from Flex
void UpdateFluidRenderBuffers(FluidRenderBuffers buffers, NvFlexSolver* solver, bool anisotropy, bool density)
{
	// use VBO buffer wrappers to allow Flex to write directly to the OpenGL buffers
	// Flex will take care of any CUDA interop mapping/unmapping during the get() operations
	if (!anisotropy)
	{
		/*std::cerr << "Draw elements\n";*/
		// regular particles
		NvFlexGetParticles(solver, buffers.mPositionBuf, buffers.mNumFluidParticles);
	}
	else
	{
		// fluid buffers
		NvFlexGetSmoothParticles(solver, buffers.mPositionBuf, buffers.mNumFluidParticles);
		NvFlexGetAnisotropy(solver, buffers.mAnisotropyBuf[0], buffers.mAnisotropyBuf[1], buffers.mAnisotropyBuf[2]);
	}

	if (density)
	{
		NvFlexGetDensities(solver, buffers.mDensitiesBuf, buffers.mNumFluidParticles);
	}
	else
	{
		NvFlexGetPhases(solver, buffers.mDensitiesBuf, buffers.mNumFluidParticles);
	}

	NvFlexGetActive(solver, buffers.mIndicesBuf);
}

void UpdateFluidRenderBuffers(FluidRenderBuffers buffers, Vec4* particles, float* densities, Vec4* anisotropy1, Vec4* anisotropy2, Vec4* anisotropy3, int numParticles, int* indices, int numIndices)
{
	// regular particles
	glVerify(glBindBuffer(GL_ARRAY_BUFFER, buffers.mPositionVBO));
	glVerify(glBufferSubData(GL_ARRAY_BUFFER, 0, buffers.mNumFluidParticles * sizeof(Vec4), particles));

	if (anisotropy1)
	{
		glVerify(glBindBuffer(GL_ARRAY_BUFFER, buffers.mAnisotropyVBO[0]));
		glVerify(glBufferSubData(GL_ARRAY_BUFFER, 0, buffers.mNumFluidParticles * sizeof(Vec4), anisotropy1));
	}

	if (anisotropy2)
	{
		glVerify(glBindBuffer(GL_ARRAY_BUFFER, buffers.mAnisotropyVBO[1]));
		glVerify(glBufferSubData(GL_ARRAY_BUFFER, 0, buffers.mNumFluidParticles * sizeof(Vec4), anisotropy2));
	}

	if (anisotropy3)
	{
		glVerify(glBindBuffer(GL_ARRAY_BUFFER, buffers.mAnisotropyVBO[2]));
		glVerify(glBufferSubData(GL_ARRAY_BUFFER, 0, buffers.mNumFluidParticles * sizeof(Vec4), anisotropy3));
	}

	// density /phase buffer
	if (densities)
	{
		glVerify(glBindBuffer(GL_ARRAY_BUFFER, buffers.mDensityVBO));
		glVerify(glBufferSubData(GL_ARRAY_BUFFER, 0, buffers.mNumFluidParticles * sizeof(float), densities));
	}

	if (indices)
	{
		glVerify(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.mIndices));
		glVerify(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, numIndices * sizeof(int), indices));
	}

	// reset
	glVerify(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	glVerify(glBindBuffer(GL_ARRAY_BUFFER, 0));

}
