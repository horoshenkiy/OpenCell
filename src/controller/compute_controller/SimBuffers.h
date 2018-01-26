#pragma once

#include <NvFlexImplFruitExt.h>

#include "../../utilits/Logger.h"
#include "../../utilits/Utilits.h"

#include "FruitSimBuffers.h"

struct Shape {
	NvFlexCollisionGeometry geometry;
	
	Vec4 position;
	Quat rotation;

	Vec4 prevPosition;
	Quat prevRotation;

	int flag;
};

class SimBuffers : public FruitSimBuffers<FruitNvFlexVector>, public Loggable {

public:

	//construtors, destructors and in initialize
	////////////////////////////////////////
	static SimBuffers& Instance(NvFlexLibrary *l);
	static SimBuffers& Get();

	SimBuffers(SimBuffers &&other) = default;
	~SimBuffers();

	void Initialize(NvFlexLibrary *l);
	void PostInitialize();

	void Reset(NvFlexLibrary *l);

	//mapping and unmapping buffers with inner buffers in Flex
	//////////////////////////////////////////////////////////
	void MapBuffers();
	void UnmapBuffers();

	// build constraints and send data		
	///////////////////////////////////////////////////////////////
	void BuildConstraints();
	void SendBuffers(NvFlexSolver *flex);

	// methods for clearing
	///////////////////////////////////////////////////////////////
	void Destroy();
	void ClearShapes();

	// methods for logging
	///////////////////////////////////////////////////////////////
	void StartLogging() override;
	void EndLogging() override;

	template<class Archive>
	void serialize(Archive &archive) {
		// global vars
		archive(numParticles, maxParticles);
		archive(numExtraParticles, numExtraMultiplier, numSolidParticles);

		// data of particles
		archive(positions,
			restPositions,
			velocities,
			phases,
			densities,
			activeIndices);

		archive(anisotropy1, anisotropy2, anisotropy3);

		archive(normals);

		archive(smoothPositions,
			diffusePositions,
			diffuseVelocities,
			diffuseIndices);

		//shape
		archive(shapeGeometry,
			shapePositions,
			shapeRotations,
			shapePrevPositions,
			shapePrevRotations,
			shapeFlags);

		//rigid
		archive(rigidOffsets,
			rigidIndices,
			rigidMeshSize,
			rigidCoefficients,
			rigidRotations,
			rigidTranslations,
			rigidLocalPositions,
			rigidLocalNormals);

		// inflatables
		archive(inflatableTriOffsets,
			inflatableTriCounts,
			inflatableVolumes,
			inflatableCoefficients,
			inflatablePressures);

		// springs
		archive(springIndices, springLengths, springStiffness);

		// dynamic triangles
		archive(triangles, triangleNormals, uvs);
	}

private:

	static bool isInitialize;

	SimBuffers(NvFlexLibrary* l);
	SimBuffers() = default;

	SimBuffers(const SimBuffers &other) = delete;
	SimBuffers operator=(const SimBuffers &other) = delete;

};
