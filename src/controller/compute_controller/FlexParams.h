#ifndef FLEX_PARAMS_H
#define FLEX_PARAMS_H

#include "NvFlex.h"
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

	//actin tree parameters
	float p_sow = 0.02f;
	float p_grow = 0.05f;
	float p_break = 0.052f;
	float p_ARP = 0.0005f;
	
	float sectionLength = 0.002f;
	float sectionRadius = 0.005f;

	float directionAngle = 0.0f;

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

#endif // FLEX_PARAMS_H


