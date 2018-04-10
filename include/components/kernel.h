#ifndef KERNEL_H
#define KERNEL_H

#include "component.h"

class Kernel : public Component {
public:

	// constructors and initialize
	Kernel() : Component() {}

	void Initialize() override;

	Vec3 GetPositionCenter() const {
		return Vec3(buffers.positions[indexCenter]);
	}

	float getRadius() {
		return trueRadius;
	}

	// update
	void Update() override {};

	template<class Archive>
	void serialize(Archive &archive) {
		archive(group, indexCenter);
	}

private:

	//fields of center
	int indexCenter = -1;

	float trueRadius;
};

#endif // KERNEL_H