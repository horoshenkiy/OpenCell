#pragma once

#include "component.h"
#include "../../../core/maths.h"
#include <random>

class Cytoskeleton;

class Branch : public Component {

private:
	Cytoskeleton *cytoskeleton;

	Shape shape;

	std::vector<Vec3> positionCapsules;
	
	//for children
	std::vector<int> indexChildBranches;
	std::vector<Branch*> childBranches;

	const float lengthCapsule = 0.005f;

	float halfLength = 0.005f;
	float radius = 0.005f;
	float fi = 1.256f;

	bool root;
	bool isLeft;

	Vec3 position;
	Vec3 positionEnd;
	
	Vec3 axesRotation = Vec3(0.0, 1.0, 0.0);
	int countFi;

	std::random_device gen;
	std::uniform_real_distribution<> urd = std::uniform_real_distribution<>(0.0f, 1.0f);

	// position of shell buffer
	int indBeginPositionShell;
	int indEndPositionShell;

	// for update branch
	void NewStraightCapsule();
	void DestroyStraightCapsule();

	void BreakBranch(int indexOfCapsule);
	void DestroyChildes(int indexCapsule);

	void NewRotationBranch();
	void NewRootBranch();

public:

	//getter
	float GetHalfLength() const {
		return halfLength;
	}
	float GetLengthCapsule() const {
		return lengthCapsule;
	}

	//initialize
	void Initialize(SimBuffers *buffers, Cytoskeleton *cytoskeleton, Vec3 position, int countFi, bool root, bool isLeft);

	//update
	void Update(bool isGrowAktin);

};
