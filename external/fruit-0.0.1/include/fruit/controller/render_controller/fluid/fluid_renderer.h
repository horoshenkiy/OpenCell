#ifndef FLUID_RENDERER_H
#define FLUID_RENDERER_H

#include <fruit/controller/render_controller/render_param.h>
#include <fruit/controller/render_controller/render_gl/shader.h>

#define STRINGIFY(A) #A

namespace FruitWork {
namespace Render {
namespace Fluid {

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

FluidRenderer* CreateFluidRenderer(uint32_t width, uint32_t height, GLuint msaaFbo);

void DestroyFluidRenderer(FluidRenderer* renderer);

}
}
}

#endif // FLUID_RENDERER_H