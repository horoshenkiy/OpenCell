#pragma once

#include "../../../external/SDL2-2.0.4/include/SDL.h"

#include "../../opengl/imguiRenderGL.h"
#include "../../opengl/shader.h"

#include "../../shaders.h"

#include "../../../core/maths.h"
#include "../../../core/platform.h"

#include "RenderParam.h"
#include "RenderBuffer.h"

#include "Camera.h"

#include "../compute_controller/FlexController.h"
#include "../compute_controller/FlexParams.h"
#include "../compute_controller/SimBuffers.h"

#include "Shadows.h"
#include "Fluid/FluidRenderer.h"

#include "../../scenes.h"

extern bool g_benchmark;

class RenderController {

private:

	//Compute params and buffers
	FlexController *flexController;
	FlexParams *flexParams;
	SimBuffers *buffers;

	//Render params and buffers
	RenderBuffers *renderBuffers;
	RenderParam *renderParam;

	//main camera
	Camera *camera;

	//////////////////////////////////////////////////////
	// Light (need cut to Render Controller)
	////////////////////////////////////////////////////////////
	Vec3 lightDir;
	Vec3 lightPos;
	Vec3 lightTarget;

	float lightDistance = 4.0f;

	////////////////////////////////////////////////////
	// private interface and fields for window
	/////////////////////////////////////////////////
	void ReshapeRender(SDL_Window* window);

	SDL_Window* window;			// window handle
	bool fullscreen = false;

	int screenWidth = 1280;
	int screenHeight = 720;

	/////////////////////////////////////////////
	//functions for rendering
	////////////////////////////////////////////////////////
	void SkinMesh();

	/////////////////////////////////////////////////////////
	// private renderers
	/////////////////////////////////////////////////////////
	FluidRenderer *fluidRenderer;
	ShadowMap *shadowMap;

	//////////////////////////////////////////////////////
	//scene
	////////////////////////////////////////////////////////
	Scene *scene;

public:

	//public fields
	//////////////////////////////////////////////
	Shadows shadows = Shadows();

	////////////////////////////////////////
	//Constructors and initialize
	/////////////////////////////////////////////
	RenderController() {}

	SDL_Window* InitRender(Camera *camera, 
							RenderParam *renderParam, 
							FlexController *flexController, 
							FlexParams *flexParams);

	//////////////////////////////////////////////
	// main render
	/////////////////////////////////////////////////
	void RenderScene(int numParticles, int numDiffuse);
	void RenderDebug();

	//костыль потому и по-русски
	void DrawShapes();

	/////////////////////////////////////////////////////
	// getters and setters
	////////////////////////////////////////////

	FluidRenderer* GetFluidRenderer() { return fluidRenderer; }

	Scene* GetScene() { return scene; }
	void SetScene(Scene *scene) { this->scene = scene; }

	ShadowMap* GetShadowMap() { return shadowMap; }
	void SetShadowMap(ShadowMap *shadowMap) { this->shadowMap = shadowMap; }

	RenderBuffers* GetRenderBuffers() { return renderBuffers; }
	void SetRenderBuffers(RenderBuffers *renderBuffers) { this->renderBuffers = renderBuffers; }

	void SetComputeBuffers(SimBuffers *buffers) { this->buffers = buffers; }

	/////////////////////////////////////////////////////////////////////
	//public interface for windows
	//////////////////////////////////////////////////////////////////////

	SDL_Window* GetWindow() { return window; }
	void SetWindow(SDL_Window* window) { this->window = window; }

	bool GetFullscreen() { return this->fullscreen; }
	void SetFullscreen(bool fullscreen) { this->fullscreen = fullscreen; }

	int GetWidth() { return this->screenWidth; }
	void SetWidth(int width) { this->screenWidth = width; }

	int GetHeight() { return this->screenHeight; }
	void SetHeight(int height) { this->screenHeight = height; }

	void ReshapeWindow(int width, int height);
	void ReshapeWindow();

	//////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////
};

