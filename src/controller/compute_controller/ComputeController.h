#pragma once 

#include "FlexController.h"
#include "FlexParams.h"
#include "SimBuffers.h"

#include "../../scenes.h"

#include "../../utilits/Timer.h"

#include "fruit_extensions/NvFlexImplFruit.h"

extern bool g_pause;
extern bool g_step;
extern int  g_frame;

extern Timer timer;

class ComputeController {
private:
	FlexController *flexController = nullptr;
	FlexParams *flexParams = nullptr;
	SimBuffers *buffers = nullptr;

	Scene *scene = nullptr;

	FruitNvFlex *fruit = new FruitNvFlex();

public:

	void Initialize();

	void PostInitialize(Scene *scene);

	void InitializeGPU();

	void MapBuffers();

	void UnmapBuffers();

	int GetActiveCount() const;

	int GetDiffuseParticles() const;

	void Update();

	void SendBuffers(NvFlexSolver *flex);

	void Sync();
};