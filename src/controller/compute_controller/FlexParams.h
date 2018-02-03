#pragma once

#include "../../../include/NvFlex.h"

class Scene;

struct FlexParams {

	FlexParams() = default;
	~FlexParams() {}

	unsigned char maxNeighborsPerParticle = 96;

	int numSubsteps = 2;

	bool warmup = false;

	// enable timers
	bool profile = false;

	// enable send help data to render
	bool interop = true;

	// flag to request collision shapes be updated
	bool shapesChanged = false;

	// flex params
	NvFlexParams params;

	void InitFlexParams(Scene *scene);
};

