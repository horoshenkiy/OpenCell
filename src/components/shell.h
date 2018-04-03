#pragma once

#include "component.h"
#include <random>

using namespace FruitWork;
namespace FruitWork { class Serializer; }

// TODO: add constructor copy
class Shell : public Component {

public:

	//constructors and initialize

	Shell() : Component() {}

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

	template<class Archive>
	void save(Archive &archive) const {
		archive(group, indBeginPosition, indEndPosition, splitThreshold);
		archive(*(asset));
	}

	template<class Archive>
	void load(Archive &archive) {
		archive(group, indBeginPosition, indEndPosition, splitThreshold);

		asset = new NvFlexExtAsset();
		archive(*(asset));
	}

private:

	friend bool operator==(const Shell&, const Shell&);

	// position in simbuffer
	size_t indBeginPosition = -1;
	size_t indEndPosition = -1;

	float splitThreshold = -1.0;

	// asset for cloth
	NvFlexExtAsset* asset = nullptr;

	void AddCloth(const Mesh* mesh, float overPressure, float invMass, int phase);

	// for random position of shell's balls
	std::random_device gen;
	std::uniform_real_distribution<float> urd = std::uniform_real_distribution<float>(-0.01f, 0.01f);
};
