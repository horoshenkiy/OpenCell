#pragma once

#include "../src/controller/compute_controller/SimBuffers.h"

/*#define COPY_VECTOR(what, from) if (from.what.size()) { \
									what.resize(from.what.size()); \
									from.what.copyto(&what[0], what.size()); \
								} else \
									what = from.what \*/

class TestSimBuffers : public SimBuffers {

public:

	TestSimBuffers(NvFlexLibrary *l) : SimBuffers(l) {}

	void SaveState(SimBuffers &buffers) {
		// global vars
		numParticles = buffers.numParticles;
		maxParticles = buffers.maxParticles;

		numExtraParticles = buffers.numExtraParticles;
		numExtraMultiplier = buffers.numExtraMultiplier;

		numSolidParticles = buffers.numSolidParticles;

		// data of particles
		CopyVector(positions, buffers.positions);
		CopyVector(restPositions, buffers.restPositions);
		CopyVector(velocities, buffers.velocities);
		CopyVector(phases, buffers.phases);
		CopyVector(densities, buffers.densities);
		CopyVector(activeIndices, buffers.activeIndices);

		CopyVector(anisotropy1, buffers.anisotropy1);
		CopyVector(anisotropy2, buffers.anisotropy2);
		CopyVector(anisotropy3, buffers.anisotropy3);

		CopyVector(normals, buffers.normals);

		CopyVector(smoothPositions, buffers.smoothPositions);
		CopyVector(diffusePositions, buffers.diffusePositions);
		CopyVector(diffuseVelocities, buffers.diffusePositions);
		CopyVector(diffuseIndices, buffers.diffuseIndices);
		
		//shape
		CopyVector(shapeGeometry, buffers.shapeGeometry);
		CopyVector(shapePositions, buffers.shapePositions);
		CopyVector(shapeRotations, buffers.shapeRotations);
		CopyVector(shapePrevPositions, buffers.shapePrevPositions);
		CopyVector(shapePrevRotations, buffers.shapePrevRotations);
		CopyVector(shapeFlags, buffers.shapeFlags);

		//rigid
		CopyVector(rigidOffsets, buffers.rigidOffsets);
		CopyVector(rigidIndices, buffers.rigidIndices);
		CopyVector(rigidMeshSize, buffers.rigidMeshSize);
		CopyVector(rigidCoefficients, buffers.rigidCoefficients);
		CopyVector(rigidRotations, buffers.rigidRotations);
		CopyVector(rigidTranslations, buffers.rigidTranslations);
		CopyVector(rigidLocalPositions, buffers.rigidLocalPositions);
		CopyVector(rigidLocalNormals, buffers.rigidLocalNormals);

		// inflatables
		CopyVector(inflatableTriOffsets, buffers.inflatableTriOffsets);
		CopyVector(inflatableTriCounts, buffers.inflatableTriCounts);
		CopyVector(inflatableVolumes, buffers.inflatableVolumes);
		CopyVector(inflatableCoefficients, buffers.inflatableCoefficients);
		CopyVector(inflatablePressures, buffers.inflatablePressures);

		// springs
		CopyVector(springIndices, buffers.springIndices);
		CopyVector(springLengths, buffers.springLengths);
		CopyVector(springStiffness, buffers.springStiffness);

		// dynamic triangles
		CopyVector(triangles, buffers.triangles);
		CopyVector(triangleNormals, buffers.triangleNormals);
		CopyVector(uvs, buffers.uvs);
	}

private:
	template<class T>
	inline void CopyVector(T &to, T &from) {
		if (!from.size())
			return;

		to.resize(from.size());
		from.copyto(&to[0], to.size());
	}
};