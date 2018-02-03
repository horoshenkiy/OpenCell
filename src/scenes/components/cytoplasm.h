#pragma once

#include "component.h"

struct FlexParams;

class Cytoplasm : public Component {

private:
	int group;
	int numberOfParticles;

public:

	// constructors and initialize
	Cytoplasm();
	Cytoplasm(int group);

	void Initialize(FlexParams *flexParams, SimBuffers *buffers);
	void PostInitialize() {}

	// getters and setters
	int GetNumberOfParticles() { return numberOfParticles; }

	// update
	void Update() {}

	void Sync() {}

	void Draw() {}
};