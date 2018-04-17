#include <fruit/primitives/primitive.h>
#include <fruit/utilits/utilits.h>

#include <ppl.h>
#include <memory.h>
#include <aabbtree.h>

namespace FruitWork {
namespace Primitives {
	
	void Cloth::CreateAABBTree()
	{
		Vec3 *massPos = new Vec3[indEndPos - indBeginPos];
	
		Concurrency::parallel_for(size_t(indBeginPos), size_t(indEndPos), [&](size_t i) {
			memcpy(&massPos[i], &buffers.positions[i], sizeof(float) * 3);
		});

		//AABBTree aabbTree(massPos, indEndPos - indBeginPos, 
		//	(uint32_t*)&buffers.triangles[indBeginTri], 
		//	indEndTri - indBeginTri);

	}
	
	
}
}
