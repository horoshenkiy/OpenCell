#pragma once

#include "../../../include/NvFlex.h"
#include "../../../include/NvFlexExt.h"
#include "../../../include/NvFlexDevice.h"

#include <iostream>

extern bool g_extensions;

void ErrorCallback(NvFlexErrorSeverity, const char* msg, const char* file, int line);

class FlexController {
private:
	NvFlexLibrary *lib;
	NvFlexSolver *solver;
	NvFlexTimers timers;
	NvFlexDetailTimer * detailTimers;

	// a setting of -1 means Flex will use the device specified in the NVIDIA control panel
	int device = -1;
	char deviceName[256];

	bool error = false;

	// vars of time


public:
	//конструктор и инициализаторы
	FlexController() {}

	int GetDevice() {
		return device;
	}
	void SetDevice(int device) {
		this->device = device;
	}

	char* GetDeviceName() {
		return deviceName;
	}

	NvFlexLibrary* GetLib() {
		return lib;
	}

	NvFlexSolver* GetSolver() {
		return solver;
	}
	void SetSolver(NvFlexSolver* solver) {
		this->solver = solver;
	}

	NvFlexTimers GetTimers() {
		return timers;
	}

	NvFlexDetailTimer* GetDetailTimers() {
		return detailTimers;
	}

	void InitFlex();
};



