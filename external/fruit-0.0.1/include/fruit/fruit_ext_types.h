#pragma once 

#include <flex/NvFlex.h>

namespace FruitWork {

struct FruitLibrary {
private:

	NvFlexLibrary *flexLibrary;

public:

	void SetLibrary(NvFlexLibrary *flexLibrary) {
		this->flexLibrary = flexLibrary;
	}
	
	NvFlexLibrary* GetLibrary() {
		return flexLibrary;
	}
};

struct FruitSolver {
private:

	NvFlexSolver *flexSolver;

public:

	void SetSolver(NvFlexSolver *flexSolver) {
		this->flexSolver = flexSolver;
	}

	NvFlexSolver* GetSolver() {
		return flexSolver;
	}
};

struct FruitBuffer {
private:

	NvFlexBuffer *flexBuffer;

public:

	void SetBuffer(NvFlexBuffer *flexBuffer) {
		this->flexBuffer = flexBuffer;
	}

	NvFlexBuffer* GetBuffer() {
		return flexBuffer;
	}
};

}