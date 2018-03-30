#pragma once

#include "component.h"

class Serializer;

class Kernel : public Component {
public:

	// constructors and initialize
	Kernel() : Component() {}

	void Initialize() override;

	// getters and setters
	Vec3 GetRateCenter() const { return rateCenter; }

	Vec3 GetPositionCenter() const {
		Vec3 result;
		result.x = positionCenter.x;
		result.y = positionCenter.y;
		result.z = positionCenter.z;

		return result;
	}

	float getRadius() {
		return trueRadius;
	}

	// update
	void Update() override;

	template<class Archive>
	void serialize(Archive &archive) {
		archive(group, indexCenter);
		archive(positionCenter, prevPositionCenter, rateCenter);
	}

private:
	
	friend bool operator==(const Kernel &lKernel, const Kernel &rKernel);

	//fields of center
	int indexCenter = -1;

	Vec4 positionCenter, prevPositionCenter;
	Vec3 rateCenter;

	float trueRadius;
};

#include <fruit/utilits/serializer.h>