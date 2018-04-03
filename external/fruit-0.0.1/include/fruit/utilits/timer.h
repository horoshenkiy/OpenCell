#ifndef TIMER_H
#define TIMER_H

#include <fruit/controller/compute_controller/flex_controller.h>
#include <flex/NvFlex.h>

#include <fruit_core/platform.h>

namespace FruitWork {
namespace Utilits {

template <class Class, typename Func, typename ... Args>
double TimeMeasurement(Class &object, Func func, Args ... args) {
	double begin = Core::FruitGetSeconds();
	(object.*func)(args ...);
	return Core::FruitGetSeconds() - begin;
}

template <typename Func, typename ... Args>
double TimeMeasurement(Func func, Args ... args) {
	double begin = Core::FruitGetSeconds();
	func(args ...);
	return Core::FruitGetSeconds() - begin;
}


class Timer {
public:

	static Timer& Get() {
		static Timer instance;
		return instance;
	}

	// vars of time
	int numDetailTimers;

	float realdt;				// the real world time delta between updates
	
	float waitTime;		// the CPU time spent waiting for the GPU
	float updateTime;     // the CPU time spent on Flex
	float renderTime;		// the CPU time spent calling OpenGL to render the scene
	float simLatency;     // the time the GPU spent between the first and last NvFlexUpdateSolver() operation. Because some GPUs context switch, this can include graphics time.

	static float GetDeviceLatency() {
		return NvFlexGetDeviceLatency(Compute::FlexController::Get().GetSolver());
	}

	/*void UpdateRealdt() {
		static double lastTime = 0;

		// real elapsed frame time
		double frameBeginTime = FruitGetSeconds();
		realdt = float(frameBeginTime - lastTime);
		lastTime = frameBeginTime;
	}*/

private:

	Timer() = default;

	Timer(const Timer &other) = delete;
	Timer operator=(const Timer &other) = delete;
};

}
}

#endif // TIMER_H