#ifndef RENDER_CONTROLLER_H
#define RENDER_CONTROLLER_H

#include <SDL.h>
#include <flex/core/maths.h>

#include <fruit/controller/render_controller/render_param.h>
#include <fruit/controller/render_controller/render_buffer.h>
#include "camera.h"
#include "fluid/fluid_renderer.h"

#include <fruit/controller/compute_controller/flex_controller.h>
#include <fruit/controller/compute_controller/flex_params.h>
#include <fruit/controller/compute_controller/sim_buffers.h>

#include <fruit/scene.h>

namespace FruitWork {
namespace Render {

class RenderController {

public:

	////////////////////////////////////////
	//Constructors and initialize
	/////////////////////////////////////////////
	void InitRender(Camera *camera);
	
	void PostInitRender(Scene *scene) {
		this->scene = scene;
		shadowMap = GL::ShadowCreate();
	}

	void Reset() {
		Compute::FlexController& flexController = Compute::FlexController::Get();
		DestroyFluidRenderBuffers(renderBuffers->fluidRenderBuffers);

		for (auto& iter : renderBuffers->meshes) {
			NvFlexDestroyTriangleMesh(flexController.GetLib(), iter.first);
			GL::DestroyGpuMesh(iter.second);
		}

		for (auto& iter : renderBuffers->fields) {
			NvFlexDestroyDistanceField(flexController.GetLib(), iter.first);
			GL::DestroyGpuMesh(iter.second);
		}

		for (auto& iter : renderBuffers->convexes) {
			NvFlexDestroyConvexMesh(flexController.GetLib(), iter.first);
			GL::DestroyGpuMesh(iter.second);
		}

		renderBuffers->fields.clear();
		renderBuffers->meshes.clear();
		renderBuffers->convexes.clear();

		delete renderBuffers->mesh;
		renderBuffers->mesh = new Mesh();
	}

	void DestroyRender() {
		for (auto& iter : renderBuffers->meshes) {
			NvFlexDestroyTriangleMesh(flexController->GetLib(), iter.first);
			Render::GL::DestroyGpuMesh(iter.second);
		}

		for (auto& iter : renderBuffers->fields) {
			NvFlexDestroyDistanceField(flexController->GetLib(), iter.first);
			Render::GL::DestroyGpuMesh(iter.second);
		}

		for (auto& iter : renderBuffers->convexes) {
			NvFlexDestroyConvexMesh(flexController->GetLib(), iter.first);
			Render::GL::DestroyGpuMesh(iter.second);
		}

		renderBuffers->fields.clear();
		renderBuffers->meshes.clear();

		if (fluidRenderer)
			Fluid::DestroyFluidRenderer(fluidRenderer);

		DestroyFluidRenderBuffers(renderBuffers->fluidRenderBuffers);

		GL::ShadowDestroy(shadowMap);
		GL::DestroyRender();
	}


	//////////////////////////////////////////////
	// main render
	/////////////////////////////////////////////////
	void Render(int numParticles, int numDiffuse) {
		GL::StartFrame(Vec4(renderParam->clearColor, 1.0f), renderParam->msaaFbo);

		RenderScene(numParticles, numDiffuse);
		RenderDebug();

		GL::EndFrame(screenWidth, screenHeight, renderParam->msaaFbo);
	}

	void RenderScene(int numParticles, int numDiffuse);
	
	void RenderDebug();

	/////////////////////////////////////////////////////
	// getters and setters
	////////////////////////////////////////////

	Fluid::FluidRenderer* GetFluidRenderer() const { return fluidRenderer; }

	Scene* GetScene() const { return scene; }
	void SetScene(Scene *scene) { this->scene = scene; }

	/////////////////////////////////////////////////////////////////////
	//public interface and values for windows
	//////////////////////////////////////////////////////////////////////

	SDL_Window* GetWindow() const { return window; }
	void SetWindow(SDL_Window* window) { this->window = window; }

	bool GetFullscreen() const { return this->fullscreen; }
	void SetFullscreen(bool fullscreen) { this->fullscreen = fullscreen; }

	PROPERTY(uint32_t, screenWidth);
	GET(screenWidth) { return _screenWidth; }

	PROPERTY(uint32_t, screenHeight);
	GET(screenHeight) { return _screenHeight; }

	void ReshapeWindow(int width, int height);
	void ReshapeWindow();

	//////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

private:

	//Compute params and buffers
	Compute::FlexController *flexController;
	Compute::FlexParams *flexParams;
	Compute::SimBuffers *buffers;

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

	uint32_t _screenWidth = 1280;
	uint32_t _screenHeight = 720;

	/////////////////////////////////////////////
	//functions for rendering
	////////////////////////////////////////////////////////
	void SkinMesh();

	/////////////////////////////////////////////////////////
	// private renderers
	/////////////////////////////////////////////////////////
	Fluid::FluidRenderer *fluidRenderer;
	GL::ShadowMap *shadowMap;

	//////////////////////////////////////////////////////
	//scene
	////////////////////////////////////////////////////////
	Scene *scene;
};

}
}

#endif // RENDER_CONTROLLER_H