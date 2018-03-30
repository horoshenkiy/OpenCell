#ifndef COMPUTE_CONTROLLER_H
#define COMPUTE_CONTROLLER_H

#include "fruit_extensions/NvFlexImplFruit.h"

#include <controller/compute_controller/flex_controller.h>
#include <controller/compute_controller/flex_params.h>
#include <controller/compute_controller/sim_buffers.h>

#include <scene.h>
#include <utilits/timer.h>

class ComputeController {

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

private:
	
	FlexController *flexController = nullptr;
	FlexParams *flexParams = nullptr;
	SimBuffers *buffers = nullptr;

	Scene *scene = nullptr;

	FruitNvFlex *fruit = new FruitNvFlex();
};

#endif // COMPUTE_CONTROLLER_H