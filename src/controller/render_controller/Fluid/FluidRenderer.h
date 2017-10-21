#pragma once

#include "../../../opengl/shader.h"

#define STRINGIFY(A) #A

#include "../RenderParam.h"
extern RenderParam *renderParam;

struct FluidRenderer
{
	GLuint mDepthFbo;
	GLuint mDepthTex;
	GLuint mDepthSmoothTex;
	GLuint mSceneFbo;
	GLuint mSceneTex;
	GLuint mReflectTex;

	GLuint mThicknessFbo;
	GLuint mThicknessTex;

	GLuint mPointThicknessProgram;
	//GLuint mPointDepthProgram;

	GLuint mEllipsoidThicknessProgram;
	GLuint mEllipsoidDepthProgram;

	GLuint mCompositeProgram;
	GLuint mDepthBlurProgram;

	int mSceneWidth;
	int mSceneHeight;
};

FluidRenderer* CreateFluidRenderer(uint32_t width, uint32_t height);

void DestroyFluidRenderer(FluidRenderer* renderer);
