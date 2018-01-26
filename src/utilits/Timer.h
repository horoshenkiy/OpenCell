#pragma once

#include "../include/NvFlex.h"

#include "../controller/compute_controller/FlexController.h"

extern FlexController flexController;

struct Timer {
	
	// vars of time
	int numDetailTimers;

	float dt = 1.0f / 120.0f;	// the time delta used for simulation

	float realdt;				// the real world time delta between updates
	float waitTime;		// the CPU time spent waiting for the GPU
	float updateTime;     // the CPU time spent on Flex
	float renderTime;		// the CPU time spent calling OpenGL to render the scene
							// the above times don't include waiting for vsync
	float simLatency;     // the time the GPU spent between the first and last NvFlexUpdateSolver() operation. Because some GPUs context switch, this can include graphics time.

	static float GetDeviceLatency() {
		return NvFlexGetDeviceLatency(flexController.GetSolver());
	}
};
