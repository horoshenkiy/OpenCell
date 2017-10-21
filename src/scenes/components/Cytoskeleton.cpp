#include "Cytoskeleton.h"
#include "Branch.h"

Cytoskeleton::Cytoskeleton(SimBuffers *buffers, int group, Vec3 start) {
	this->buffers = buffers;
	this->group = group;

	start.x += 0.1;
	start.y = 0.15;
	this->start = start;
}

void Cytoskeleton::AddBranch(Branch *branch) {
	branches.push_back(branch);
}

void Cytoskeleton::AddShape(Shape shape) {
	shapes.push_back(shape);
}

void Cytoskeleton::Initialize(Cell *cell, SimBuffers *buffers) {
	this->cell = cell;
	this->buffers = buffers;

	Vec3 position = start;

	branches.push_back(new Branch(0));
	branches.back()->Initialize(buffers, this, start, 0, true, true);

	position.x += 0.1;
	position.z += 0.2;
	branches.push_back(new Branch(0));
	branches.back()->Initialize(buffers, this, position, 0, true, true);

	position = start;
	position.x += 0.1;
	position.z -= 0.2;
	branches.push_back(new Branch(0));
	branches.back()->Initialize(buffers, this, position, 0, true, true);
}

void Cytoskeleton::Update(Vec3 rateKernelCenter) {

	// rate of grow
	bool isGrowAktin; 
	if (itGrow < rateGrow) {
		itGrow++;
		isGrowAktin = false;
	}
	else {
		itGrow = 0;
		isGrowAktin = true;
	}

	this->ratePointGrow = rateKernelCenter;

	shapes.clear();

	// buffers->ClearShapes()
	buffers->shapeGeometry.resize(0);
	
	buffers->shapePositions.resize(0);
	buffers->shapeRotations.resize(0);

	buffers->shapePrevPositions.resize(0);
	buffers->shapePrevRotations.resize(0);

	buffers->shapeFlags.resize(0);

	for (int i = 0; i < branches.size(); i++) {
		branches[i]->Update(isGrowAktin);
	}

	for (int i = 0; i < shapes.size(); i++) {
		buffers->shapeGeometry.push_back(shapes[i].geometry);
		
		buffers->shapePositions.push_back(shapes[i].position);
		buffers->shapeRotations.push_back(shapes[i].rotation);

		buffers->shapePrevPositions.push_back(shapes[i].prevPosition);
		buffers->shapePrevRotations.push_back(shapes[i].prevRotation);

		buffers->shapeFlags.push_back(shapes[i].flag);
	}
}