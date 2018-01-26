#pragma once

#include <NvFlexImplFruitExt.h>

#include "../../Logger.h"
#include "../../Utilits.h"

#include "FruitSimBuffers.h"

struct Shape {
	NvFlexCollisionGeometry geometry;
	
	Vec4 position;
	Quat rotation;

	Vec4 prevPosition;
	Quat prevRotation;

	int flag;
};

struct SimBuffers : public FruitSimBuffers<FruitNvFlexVector>, public Loggable {
	//construtors, destructors and in initialize
	////////////////////////////////////////
	SimBuffers(NvFlexLibrary* l);
	~SimBuffers();

	void Initialize();
	void PostInitialize();

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
};