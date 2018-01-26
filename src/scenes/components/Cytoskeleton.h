#pragma once

#include "component.h"
#include "Branch.h"

class Cell;

class Cytoskeleton : public Component {

public:

	// need incapsulation
	Cell *cell = nullptr;

	Cytoskeleton() : Component() {};

	Cytoskeleton(Cell *cell, Vec3 start);

	// getters
	Vec3 GetRatePointGrow() const {
		return ratePointGrow;
	}

	////////////////////////////

	void AddBranch(Branch *branch);

	void AddShape(Shape shape);

	void Initialize(Cell *cell, SimBuffers *buffers);

	void Update(Vec3 rateKernelCenter);

	int indexUpdateCapsule = 0;

private:

	std::vector<Shape> shapes;

	std::vector<Branch*> branches;

	Vec3 start = Vec3(2.9f, 0.2f, 1.85f);
	
	int group = -1;

	Vec3 ratePointGrow = Vec3(0.003f, 0.0f, 0.0f);
	const int rateGrow = 0;
	int itGrow = 0;



};
