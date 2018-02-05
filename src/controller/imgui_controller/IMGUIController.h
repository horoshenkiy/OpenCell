#ifndef IMGUI_CONTROLLER_H
#define IMGUI_CONTROLLER_H

#include "../SDLController.h"

#include "../compute_controller/FlexController.h"
#include "../compute_controller/FlexParams.h"
#include "../compute_controller/SimBuffers.h"

#include "../render_controller/RenderController.h"

#include "imgui.h"

#include "../../scenes.h"
#include "../../utilits/Timer.h"
#include "../../utilits/Video.h"

extern int g_frame;

extern Timer timer;
extern Video video;

class IMGUIController {
private:
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

	// do statistics and menu
	void DoStatistic(int numParticles, int numDiffuse);
	void DoMenu();

	// render GUI
	void Draw();

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
};

#endif