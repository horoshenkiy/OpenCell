#ifndef IMGUI_CONTROLLER_H
#define IMGUI_CONTROLLER_H

#include <fruit/controller/sdl_controller.h>
#include <fruit/controller/compute_controller/flex_controller.h>
#include <fruit/controller/compute_controller/flex_params.h>
#include <fruit/controller/compute_controller/sim_buffers.h>
#include <fruit/controller/render_controller/render_controller.h>

#include <fruit/scene.h>

namespace FruitWork {

class IMGUIController {
public:

	// initialize
	void Initialize(Scene *scene,
					FlexController *flexController, 
					FlexParams *flexParams,
					RenderController *renderController, 
					RenderParam *renderParam, 
					SDLController *sdlController);

	//setters and getters
	void SetTweakPanel(bool tweakPanel) { this->tweakPanel = tweakPanel; }

	// public method for create menu
	int DoUI(int numParticles, int numDiffuse);

private:
	
	// do statistics and menu
	void DoStatistic(int numParticles, int numDiffuse);
	void DoMenu();

	// render GUI
	void Draw();

	int x, y;
	int fontHeight;

	Scene *scene;

	FlexController *flexController;
	RenderController *renderController;
	SDLController *sdlController;

	FlexParams *flexParams;
	RenderParam *renderParam;

	SimBuffers *buffers;

	bool tweakPanel = true;
};

}

#endif