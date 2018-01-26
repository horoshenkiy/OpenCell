#pragma once

#include "Primitives.h"

class FactoryRigidCapsule {

public:

	// constructors and destructors
	FactoryRigidCapsule() = default;
	~FactoryRigidCapsule() = default;

	FactoryRigidCapsule(const FactoryRigidCapsule &other) = delete;
	FactoryRigidCapsule(FactoryRigidCapsule && other) = delete;

	FactoryRigidCapsule operator=(const FactoryRigidCapsule &other) = delete;

	// API
	RigidCapsule CreateCapsule();

private:
};