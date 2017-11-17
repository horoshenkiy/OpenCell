#pragma once

#include "component.h"
#include <random>

class Serializer;

// TODO: add constructor copy
class Shell : public Component {

public:

	//constructors and initialize

	explicit Shell(SimBuffers *buffers) : Component(buffers, nullptr) {}

	explicit Shell(int group, SimBuffers *buffers) : Component(buffers ,nullptr), group(group) {}

	explicit Shell(const Shell& other) : Component(other.buffers, nullptr), group(other.group) {}

	void Initialize() override;

	// destructor
	~Shell() {
		NvFlexExtDestroyAsset(asset);
	}

	// getters and setters
	size_t GetIndBeginPosition() const { return indBeginPosition; }

	size_t GetIndEndPosition() const { return indEndPosition; }

	// update
	void Update() override;

	void Draw() override {
		//DrawCloth(&g_buffers->positions[0], &g_buffers->normals[0], NULL, &g_buffers->triangles[0], asset->numTriangles, g_buffers->positions.size(), (0 + 2) % 6);
	}

private:

	friend Serializer;
	friend bool operator==(const Shell&, const Shell&);

	int group = -1;

	// position in simbuffer
	size_t indBeginPosition = -1;
	size_t indEndPosition = -1;

	float splitThreshold = -1.0;

	// asset for cloth
	NvFlexExtAsset* asset = nullptr;

	void AddInflatable(const Mesh* mesh, float overPressure, float invMass, int phase);

	// for random position of shell's balls
	std::random_device gen;
	std::uniform_real_distribution<float> urd = std::uniform_real_distribution<float>(-0.01f, 0.01f);
};
