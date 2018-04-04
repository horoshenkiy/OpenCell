#ifndef KERNEL_H
#define KERNEL_H

#include "component.h"

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

	//fields of center
	int indexCenter = -1;

	Vec4 positionCenter, prevPositionCenter;
	Vec3 rateCenter;

	float trueRadius;
};

#endif // KERNEL_H