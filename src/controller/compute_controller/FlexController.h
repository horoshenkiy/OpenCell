#pragma once

#include "../../../include/NvFlex.h"

extern bool g_extensions;

void ErrorCallback(NvFlexErrorSeverity, const char* msg, const char* file, int line);

class FlexController {
private:
	NvFlexLibrary *lib = nullptr;
	NvFlexSolver *solver = nullptr;
	NvFlexTimers timers;
	NvFlexDetailTimer * detailTimers = nullptr;

	// a setting of -1 means Flex will use the device specified in the NVIDIA control panel
	int device = -1;
	char deviceName[256];

	bool error = false;

	// vars of time


public:
	//конструктор и инициализаторы
	FlexController() = default;

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

	void InitFlex();
};



