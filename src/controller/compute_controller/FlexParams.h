#pragma once

#include "../../../include/NvFlex.h"
#include <iostream>

class Scene;

class FlexParams {

public:

	static FlexParams& Instance() {
		static FlexParams instance;
		return instance;
	}

	static FlexParams& Get() {
		return Instance();
	}

	~FlexParams() = default;

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

	//void InitFlexParams(Scene *scene);

private:

	FlexParams();

	FlexParams(const FlexParams &other) = delete;
	FlexParams operator=(const FlexParams &other) = delete;

};

