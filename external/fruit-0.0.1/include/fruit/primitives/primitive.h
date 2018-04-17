#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <flex/NvFlex.h>
#include <flex/NvFlexExt.h>

#include <fruit/controller/compute_controller/sim_buffers.h>

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

		Cloth() : 
		buffers(Compute::SimBuffers::Get()) 
		{}

		Cloth(NvFlexExtAsset *asset_,
			size_t indBegin, size_t indEnd,
			size_t indBeginTri, size_t indEndTri)
			:
			buffers(Compute::SimBuffers::Get()),
			asset(asset_),
			indBeginPos(indBegin), indEndPos(indEnd),
			indBeginTri(indBeginTri), indEndTri(indEndTri)
		{}

		~Cloth() {
			if (asset) NvFlexExtDestroyAsset(asset);
		}

		size_t GetIndBeginPos() const {
			return indBeginPos;
		}

		size_t GetIndEndPos() const {
			return indEndPos;
		}

		void AddFluctuations(float min, float max) {
			for (size_t i = indBeginPos; i < indEndPos; i++)
				buffers.positions[i] += { Randf(min, max), Randf(min, max), Randf(min, max), 0.f };
		}

		void CreateAABBTree();

	private:

		size_t indBeginPos = -1, indEndPos = -1, indBeginTri = -1, indEndTri = -1;

		// asset for cloth
		NvFlexExtAsset* asset = nullptr;

		// sim_buffers
		Compute::SimBuffers &buffers;
	};
}
}
#endif // PRIMITIVE_H