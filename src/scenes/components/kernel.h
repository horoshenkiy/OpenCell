#pragma once

#include "component.h"

class Serializer;

class Kernel : public Component {
public:

	// constructors and initialize
	Kernel() = default;

	Kernel(SimBuffers *buffers, RenderBuffers *renderBuffers) : Component(buffers, renderBuffers) {}

	Kernel(int group, SimBuffers *buffers, RenderBuffers *renderBuffers) : Component(buffers, renderBuffers) {
		this->group = group;
	}																	  

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

	// update
	void Update() override;

	template<class Archive>
	void serialize(Archive &archive) {
		archive(group, indexCenter);
		archive(positionCenter, prevPositionCenter, rateCenter);
	}

private:
	
	friend bool operator==(const Kernel &lKernel, const Kernel &rKernel);

	int group = -1;

	//fields of center
	int indexCenter = -1;

	Vec4 positionCenter, prevPositionCenter;
	Vec3 rateCenter;

};

#include "../../Serializer.h"