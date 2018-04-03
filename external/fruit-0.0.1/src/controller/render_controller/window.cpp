#include <controller/imgui_controller/imgui_render_gl.h>
#include <controller/render_controller/render_controller.h>
#include <application.h>

namespace FruitWork {
namespace Render {

void RenderController::InitRender(Camera *camera) {

	// TODO: control for initialize
	
	this->camera = camera;
	
	this->renderBuffers = &RenderBuffers::Get();
	this->renderParam = &RenderParam::Get();

	this->flexController = &Compute::FlexController::Get();
	this->flexParams = &Compute::FlexParams::Get();
	this->buffers = &Compute::SimBuffers::Get();

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

	// Turn on double buffering with a 24bit Z buffer.
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_GL_CreateContext(window);

	// This makes our buffer swap syncronized with the monitor's vertical refresh
	SDL_GL_SetSwapInterval(1);

	glewExperimental = GL_TRUE;
	glewInit();

	IMGUI::GL::imguiRenderGLInit("../../data/DroidSans.ttf");
	ReshapeWindow();
}

void RenderController::ReshapeWindow(int width, int height)
{
	if (!AppParams::g_benchmark)
		printf("Reshaping\n");

	ReshapeRender(window);

	if (!fluidRenderer || (width != this->screenWidth || height != this->_screenHeight))
	{
		if (fluidRenderer)
			DestroyFluidRenderer(fluidRenderer);
		fluidRenderer = Fluid::CreateFluidRenderer(width, height, renderParam->msaaFbo);
	}

	_screenWidth = width;
	_screenHeight = height;
}

void RenderController::ReshapeWindow() {
	if (!AppParams::g_benchmark)
		printf("Reshaping\n");

	ReshapeRender(window);

	if (fluidRenderer)
		DestroyFluidRenderer(fluidRenderer);
	fluidRenderer = Fluid::CreateFluidRenderer(screenWidth, _screenHeight, renderParam->msaaFbo);
}

void RenderController::ReshapeRender(SDL_Window* window)
{
	int width, height;
	SDL_GetWindowSize(window, &width, &height);

	if (renderParam->msaaSamples)
	{
		glVerify(glBindFramebuffer(GL_FRAMEBUFFER, 0));

		if (renderParam->msaaFbo)
		{
			glVerify(glDeleteFramebuffers(1, &renderParam->msaaFbo));
			glVerify(glDeleteRenderbuffers(1, &renderParam->msaaColorBuf));
			glVerify(glDeleteRenderbuffers(1, &renderParam->msaaDepthBuf));
		}

		int samples;
		glGetIntegerv(GL_MAX_SAMPLES_EXT, &samples);

		// clamp samples to 4 to avoid problems with point sprite scaling
		samples = std::min(samples, std::min(renderParam->msaaSamples, 4));

		glVerify(glGenFramebuffers(1, &renderParam->msaaFbo));
		glVerify(glBindFramebuffer(GL_FRAMEBUFFER, renderParam->msaaFbo));

		glVerify(glGenRenderbuffers(1, &renderParam->msaaColorBuf));
		glVerify(glBindRenderbuffer(GL_RENDERBUFFER, renderParam->msaaColorBuf));
		glVerify(glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGBA8, width, height));

		glVerify(glGenRenderbuffers(1, &renderParam->msaaDepthBuf));
		glVerify(glBindRenderbuffer(GL_RENDERBUFFER, renderParam->msaaDepthBuf));
		glVerify(glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT, width, height));
		glVerify(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderParam->msaaDepthBuf));

		glVerify(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderParam->msaaColorBuf));

		glVerify(glCheckFramebufferStatus(GL_FRAMEBUFFER));

		glEnable(GL_MULTISAMPLE);
	}
}

}
}