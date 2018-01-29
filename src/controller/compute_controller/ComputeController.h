#pragma once 

#include "FlexController.h"
#include "FlexParams.h"
#include "SimBuffers.h"

#include "../../scenes.h"

#include "../../utilits/Timer.h"

extern bool g_pause;
extern bool g_step;
extern int  g_frame;

extern Timer timer;

class ComputeController {
private:
	FlexController *flexController = nullptr;
	FlexParams *flexParams = nullptr;
	SimBuffers *buffers = nullptr;

	// need to compute diffuse particles
	RenderParam *renderParam = nullptr;

	Scene *scene = nullptr;

	Fruit *fruit = new FruitNvFlex();

public:

	void Initialize(FlexController *flexController, FlexParams *flexParams, RenderParam *renderParam, Scene *scene);

	int GetActiveCount() const;

	int GetDiffuseParticles() const;

	void Update();
};