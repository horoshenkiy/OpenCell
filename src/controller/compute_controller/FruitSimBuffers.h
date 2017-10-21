#pragma once

#include "../../../fruit_extensions/NvFlexImplFruit.h"

template <template<class> class TypeVector>
struct FruitSimBuffers 
{
	// interface for Flex
	Fruit *fruit = new FruitNvFlex();

	// global vars
	uint32_t numParticles;
	uint32_t maxParticles;
	const int maxDiffuseParticles = 0;

	int numExtraParticles = 0;
	int numExtraMultiplier = 1;

	int numSolidParticles = 0;

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

	FruitSimBuffers() {};

	//void Initialize() = 0;
};