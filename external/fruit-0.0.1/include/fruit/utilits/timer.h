#ifndef TIMER_H
#define TIMER_H

#include <fruit/controller/compute_controller/flex_controller.h>
#include <flex/NvFlex.h>

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
		return NvFlexGetDeviceLatency(FlexController::Instance().GetSolver());
	}

	/*void UpdateRealdt() {
		static double lastTime = 0;

		// real elapsed frame time
		double frameBeginTime = FruitGetSeconds();
		realdt = float(frameBeginTime - lastTime);
		lastTime = frameBeginTime;
	}*/
};

#endif // TIMER_H