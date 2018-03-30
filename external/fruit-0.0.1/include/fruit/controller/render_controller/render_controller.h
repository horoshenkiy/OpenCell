#ifndef RENDER_CONTROLLER_H
#define RENDER_CONTROLLER_H

#include <SDL.h>
#include <flex/core/maths.h>

// temp
#include <fruit/controller/render_controller/render_param.h>
#include <fruit/controller/render_controller/render_buffer.h>
///
#include "camera.h"
#include "shadows.h"
#include "fluid/fluid_renderer.h"

#include <fruit/controller/compute_controller/flex_controller.h>
#include <fruit/controller/compute_controller/flex_params.h>
#include <fruit/controller/compute_controller/sim_buffers.h>

#include <fruit/scene.h>

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

#endif // RENDER_CONTROLLER_H