#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <quat.h>
#include <vec4.h>
#include <flex/NvFlex.h>

namespace FruitWork {
namespace Primitives {
		
	struct Shape {
		NvFlexCollisionGeometry geometry;

		Vec4 position;
		XQuat<float> rotation;

		Vec4 prevPosition;
		XQuat<float> prevRotation;

		int flag;
	};

	class RigidCapsule {
	public:

		RigidCapsule() = default;
		~RigidCapsule() = default;

		RigidCapsule(const RigidCapsule &other) = delete;
		RigidCapsule(RigidCapsule && other) = default;

		RigidCapsule operator=(const RigidCapsule &other) = delete;

	};
}
}
#endif // PRIMITIVE_H