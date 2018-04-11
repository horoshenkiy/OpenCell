#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <quat.h>
#include <vec4.h>

#include <flex/NvFlex.h>
#include <flex/NvFlexExt.h>

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

	// wrapper for NVidia Flex Cloth
	class Cloth {
	public:

		Cloth() = default;

		Cloth(NvFlexExtAsset *asset_, size_t indBegin, size_t indEnd) : 
			asset(asset_), indBeginPos(indBegin), indEndPos(indEnd) 
		{}

		~Cloth() {
			if (asset)
				NvFlexExtDestroyAsset(asset);
		}

		size_t indBeginPos, indEndPos;

		// asset for cloth
		NvFlexExtAsset* asset = nullptr;

	private:

		
	};
}
}
#endif // PRIMITIVE_H