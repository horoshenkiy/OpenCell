#pragma once

class RigidCapsule {

public:

	RigidCapsule() = default;
	~RigidCapsule() = default;

	RigidCapsule(const RigidCapsule &other) = delete;
	RigidCapsule(RigidCapsule && other) = default;

	RigidCapsule operator=(const RigidCapsule &other) = delete;

private:


};