#pragma once

#include "component.h"
#include "Branch.h"
#include "Kernel.h"

class Cell;

class Cytoskeleton : public Component {

	std::vector<Shape> shapes;

	std::vector<Branch*> branches;

	Vec3 start = Vec3(2.9f, 0.2f, 1.85f);

	int group;

	Vec3 ratePointGrow = Vec3(0.003, 0.0, 0.0);
	const int rateGrow = 0;
	int itGrow = 0;

public:

	// need incapsulation
	Cell *cell;

	Cytoskeleton() {};

	Cytoskeleton(SimBuffers *buffers, int group, Vec3 start);

	// getters
	Vec3 GetRatePointGrow() {
		return ratePointGrow;
	}

	////////////////////////////

	void AddBranch(Branch *branch);

	void AddShape(Shape shape);

	void Initialize(Cell *cell, SimBuffers *buffers);

	void Update(Vec3 rateKernelCenter);

	int indexUpdateCapsule;

};
