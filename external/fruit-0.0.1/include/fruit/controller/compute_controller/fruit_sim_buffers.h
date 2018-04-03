#ifndef FRUIT_SIM_BUFFERS_H
#define FRUIT_SIM_BUFFERS_H

#include <fruit/utilits/property.h>

namespace FruitWork {
namespace Compute {

// rename to SimBuffers
template <template<class> class TypeVector>
struct FruitSimBuffers 
{
	// global vars
	size_t numParticles = 0;
	size_t maxParticles = 0;
	const size_t maxDiffuseParticles = 0;

	size_t numExtraParticles = 0;
	size_t numExtraMultiplier = 1;

	size_t numSolidParticles = 0;

	// data of particles
	TypeVector<Vec4> positions;
	TypeVector<Vec4> restPositions;
	TypeVector<Vec3> velocities;
	TypeVector<int>  phases;
	TypeVector<float> densities;
	TypeVector<int> activeIndices;

	TypeVector<Vec4> anisotropy1;
	TypeVector<Vec4> anisotropy2;
	TypeVector<Vec4> anisotropy3;

	TypeVector<Vec4> normals;
	
	TypeVector<Vec4> smoothPositions;
	TypeVector<Vec4> diffusePositions;
	TypeVector<Vec4> diffuseVelocities;
	TypeVector<int>  diffuseIndices;

	//shape
	TypeVector<NvFlexCollisionGeometry> shapeGeometry;
	TypeVector<Vec4> shapePositions;
	TypeVector<Quat> shapeRotations;
	TypeVector<Vec4> shapePrevPositions;
	TypeVector<Quat> shapePrevRotations;
	TypeVector<int> shapeFlags;

	//rigid
	TypeVector<int> rigidOffsets;
	TypeVector<int> rigidIndices;
	TypeVector<int> rigidMeshSize;
	TypeVector<float> rigidCoefficients;
	TypeVector<Quat> rigidRotations;
	TypeVector<Vec3> rigidTranslations;
	TypeVector<Vec3> rigidLocalPositions;
	TypeVector<Vec4> rigidLocalNormals;

	// inflatables
	TypeVector<int> inflatableTriOffsets;
	TypeVector<int> inflatableTriCounts;
	TypeVector<float> inflatableVolumes;
	TypeVector<float> inflatableCoefficients;
	TypeVector<float> inflatablePressures;

	// springs
	TypeVector<int> springIndices;
	TypeVector<float> springLengths;
	TypeVector<float> springStiffness;

	// dynamic triangles
	TypeVector<int> triangles;
	TypeVector<Vec3> triangleNormals;
	TypeVector<Vec3> uvs;

};

}
}

#endif // FRUIT_SIM_BUFFERS_H