#pragma once

#include "component.h"

class Kernel : public Component {
private:

	int group;

	//fields of center
	int indexCenter;
	
	Vec4 positionCenter, prevPositionCenter;
	Vec3 rateCenter;

	float trueRadius;

public:

	// constructors and initialize
	Kernel() {}
	Kernel(int group) { this->group = group; }

	void Initialize(SimBuffers *buffers, RenderBuffers *renderBuffers);
	void PostInitialize() {}

	// getters and setters
	Vec3 GetRateCenter() { return rateCenter; }

	Vec3 GetPositionCenter() {
		Vec3 result;
		result.x = positionCenter.x;
		result.y = positionCenter.y;
		result.z = positionCenter.z;

		return result;
	}

	float getRadius()
	{
		return trueRadius;
	}

	// update
	void Update();

	void Sync() {}

	void Draw() {}
};