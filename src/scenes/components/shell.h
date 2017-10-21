#pragma once

#include "component.h"
#include <random>

class Shell : public Component {

private:

	int group;
	
	// position in simbuffer
	int indBeginPosition;
	int indEndPosition;

	float splitThreshold;

	// asset for cloth
	NvFlexExtAsset* asset;

	void AddInflatable(const Mesh* mesh, float overPressure, float invMass, int phase);

	// for random position of shell's balls
	std::random_device gen;
	std::uniform_real_distribution<> urd = std::uniform_real_distribution<>(-0.01f, 0.01f);

public:

	//constructors and initialize
	Shell() {};
	Shell(int group) { this->group = group; }

	void Initialize(SimBuffers *buffers);
	void PostInitialize() {}

	// destructor
	~Shell() {
		NvFlexExtDestroyTearingCloth(asset);
	}

	// getters and setters
	int GetIndBeginPosition() { return indBeginPosition; }

	int GetIndEndPosition() { return indEndPosition; }

	// update
	void Update();

	void Sync() {}

	void Draw() {
		//DrawCloth(&g_buffers->positions[0], &g_buffers->normals[0], NULL, &g_buffers->triangles[0], asset->numTriangles, g_buffers->positions.size(), (0 + 2) % 6);
	}
};