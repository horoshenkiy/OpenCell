#pragma once

#include "../../../external/SDL2-2.0.4/include/SDL.h"
#include "../../../core/maths.h"

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

	uint32_t screenWidth = 1280;
	uint32_t screenHeight = 720;

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

	FlexParams *flexParams;

	//public fields
	//////////////////////////////////////////////
	Shadows shadows = Shadows();

	////////////////////////////////////////
	//Constructors and initialize
	/////////////////////////////////////////////
	RenderController() = default;

	void InitRender(Camera *camera);

	//////////////////////////////////////////////
	// main render
	/////////////////////////////////////////////////
	void RenderScene(int numParticles, int numDiffuse);
	void RenderDebug();

	//костыль потому и по-русски
	void DrawShapes() const;

	/////////////////////////////////////////////////////
	// getters and setters
	////////////////////////////////////////////

	FluidRenderer* GetFluidRenderer() const { return fluidRenderer; }

	Scene* GetScene() const { return scene; }
	void SetScene(Scene *scene) { this->scene = scene; }

	ShadowMap* GetShadowMap() const { return shadowMap; }
	void SetShadowMap(ShadowMap *shadowMap) { this->shadowMap = shadowMap; }

	/////////////////////////////////////////////////////////////////////
	//public interface for windows
	//////////////////////////////////////////////////////////////////////

	SDL_Window* GetWindow() const { return window; }
	void SetWindow(SDL_Window* window) { this->window = window; }

	bool GetFullscreen() const { return this->fullscreen; }
	void SetFullscreen(bool fullscreen) { this->fullscreen = fullscreen; }

	uint32_t GetWidth() const { return this->screenWidth; }
	void SetWidth(uint32_t width) { this->screenWidth = width; }

	uint32_t GetHeight() const { return this->screenHeight; }
	void SetHeight(uint32_t height) { this->screenHeight = height; }

	void ReshapeWindow(int width, int height);
	void ReshapeWindow();

	//////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////
};

