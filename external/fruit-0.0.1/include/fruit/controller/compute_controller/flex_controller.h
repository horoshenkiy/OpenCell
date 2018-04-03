#ifndef FLEX_CONTROLLER_H
#define FLEX_CONTROLLER_H

#include <flex/NvFlex.h>

namespace FruitWork {
namespace Compute {

void ErrorCallback(NvFlexErrorSeverity, const char* msg, const char* file, int line);

class FlexController {

public:

	static FlexController& Instance() {
		static FlexController instance;
		return instance;
	}

	static FlexController& Get() {
		return Instance();
	}

	int GetDevice() const {
		return device;
	}
	void SetDevice(int device) {
		this->device = device;
	}

	char* GetDeviceName() {
		return deviceName;
	}

	NvFlexLibrary* GetLib() const {
		return lib;
	}

	NvFlexSolver* GetSolver() const {
		return solver;
	}
	void SetSolver(NvFlexSolver* solver) {
		this->solver = solver;
	}

	NvFlexTimers GetTimers() const {
		return timers;
	}

	NvFlexDetailTimer* GetDetailTimers() const {
		return detailTimers;
	}

	//void InitFlex();

private:

	FlexController();

	FlexController(const FlexController &other) = delete;
	FlexController operator=(const FlexController &other) = delete;

	NvFlexLibrary *lib = nullptr;
	NvFlexSolver *solver = nullptr;
	NvFlexTimers timers;
	NvFlexDetailTimer * detailTimers = nullptr;

	// a setting of -1 means Flex will use the device specified in the NVIDIA control panel
	int device = -1;
	char deviceName[256];

	bool error = false;
};

}
}

#endif // FLEX_CONTROLLER_H