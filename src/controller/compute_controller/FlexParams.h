#pragma once

#include "../../../include/NvFlex.h"

#include "../../../core/mesh.h"

#include <iostream>

class Scene;

struct FlexParams {

	unsigned char maxNeighborsPerParticle = 96;

	int numSubsteps = 2;

	bool warmup = false;

	//actin tree parameters
	float p_sow = 0.02f;
	float p_grow = 0.05f;
	float p_break = 0.052f;
	float p_ARP = 0.0005f;

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

