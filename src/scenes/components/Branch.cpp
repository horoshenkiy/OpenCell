#include "Branch.h"
#include "Cytoskeleton.h"
#include "../Cell.h"

#include "../../controller/compute_controller/SimBuffers.h"
#include "../../Utilits.h"

void Branch::Initialize(SimBuffers *buffers, Cytoskeleton *cytoskeleton, Vec3 position, int countFi, bool root, bool isLeft) {
	this->buffers = buffers;

	this->indBeginPositionShell = cytoskeleton->cell->shell->GetIndBeginPosition();
	this->indEndPositionShell = cytoskeleton->cell->shell->GetIndEndPosition();

	this->position = position;
	this->countFi = countFi;
	this->fi = (isLeft) ? fi : -fi;

	this->root = root;
	this->isLeft = isLeft;

	this->cytoskeleton = cytoskeleton;

	// create first capsule
	this->position.x += lengthCapsule * Cos(-fi * countFi) * 2;
	this->position.z += lengthCapsule * Sin(-fi * countFi) * 2;

	Quat rotation = QuatFromAxisAngle(axesRotation, fi * countFi);
	this->shape = AddCapsule(radius, halfLength, this->position, rotation);
	halfLength += lengthCapsule;

	positionCapsules.push_back(this->position);

	// add shape 
	cytoskeleton->AddShape(this->shape);
}

void Branch::Update(bool isGrowAktin) {

	// for rate
	Vec3 vecGrow = cytoskeleton->GetRatePointGrow();

	position.x += vecGrow.x;
	position.z += vecGrow.z;

	Quat rotation = QuatFromAxisAngle(axesRotation, fi * countFi);
	this->shape = ResizeCapsule(this->shape, radius, halfLength, position, rotation);

	std::for_each(positionCapsules.begin(), positionCapsules.end(), [vecGrow](Vec3 &el) { el += vecGrow; });

	for (int j = 0; j < childBranches.size(); j++) {
		childBranches[j]->Update(isGrowAktin);
	}

	cytoskeleton->AddShape(this->shape);

	////////////////////////////////////////////

	if (!isGrowAktin)
		return;

	///////////////////////////////////////////////////////
	// break of branch
	double ksi;
	for (int j = 0; j < positionCapsules.size(); j++) {
		ksi = urd(gen);

		if (ksi <= 0.0005) {
			DestroyChildes(j);
			BreakBranch(j);
		}
	}

	// is it collises with shell?
	//////////////////////////////////////
	positionEnd = position;
	positionEnd.x += halfLength * Cos(-fi * countFi);
	positionEnd.z += halfLength * Sin(-fi * countFi);

	float minDist = FindMinDistToSet(positionEnd, buffers->positions, indBeginPositionShell, indEndPositionShell);
	if (minDist < 0.05)
		return;

	///////////////////////////////////

	ksi = urd(gen);
	if (ksi <= 0.5f) {
		if (ksi > 0.03f) {
			NewStraightCapsule();
		}
		else if (root) {
			NewRotationBranch();
		}
		else {
			NewRootBranch();
		}
	}
}

void Branch::NewStraightCapsule() {

	// add capsule
	positionCapsules.push_back(this->position);

	position.x += lengthCapsule * Cos(-fi * countFi);
	position.z += lengthCapsule * Sin(-fi * countFi);

	halfLength += lengthCapsule;

	Quat rotation = QuatFromAxisAngle(axesRotation, fi * countFi);
	this->shape = ResizeCapsule(this->shape, radius, halfLength, position, rotation);
}

void Branch::BreakBranch(int indexOfCapsule) {

	halfLength = lengthCapsule + indexOfCapsule * lengthCapsule;
	position = positionCapsules[indexOfCapsule];

	Quat rotation = QuatFromAxisAngle(axesRotation, fi * countFi);
	this->shape = ResizeCapsule(this->shape, radius, halfLength, position, rotation);

	positionCapsules.resize(indexOfCapsule);
}

void Branch::DestroyChildes(int indexCapsule) {

	size_t index = childBranches.size();
	for (size_t i = 0; i < childBranches.size(); i++) {
		if (indexCapsule <= indexChildBranches[i]) {
			index = i;
			break;
		}
	}

	childBranches.resize(index);
	indexChildBranches.resize(index);
}

void Branch::DestroyStraightCapsule() {
	halfLength -= lengthCapsule;

	position.x += lengthCapsule * Cos(-fi * countFi);
	position.z += lengthCapsule * Sin(-fi * countFi);

	Quat rotation = QuatFromAxisAngle(axesRotation, fi * countFi);
	this->shape = ResizeCapsule(this->shape, radius, halfLength, position, rotation);
}

void Branch::NewRotationBranch() {
	Vec3 pos = position;

	float ksi = rand() % 100 + 1.0f;
	if (ksi <= 50) {
		isLeft = true;
	}
	else {
		pos.x += lengthCapsule * Cos(-fi * (countFi + 1));
		pos.z += lengthCapsule * Cos(-fi * (countFi + 1));

		isLeft = false;
	}

	Branch *branch = new Branch();
	branch->Initialize(buffers, cytoskeleton, pos, 1, false, isLeft);

	childBranches.push_back(branch);
	indexChildBranches.push_back(positionCapsules.size() - 1);
}


void Branch::NewRootBranch() {

	Branch *branch = new Branch();
	branch->Initialize(buffers, cytoskeleton, position, 0, true, isLeft);

	childBranches.push_back(branch);
	indexChildBranches.push_back(positionCapsules.size() - 1);
}