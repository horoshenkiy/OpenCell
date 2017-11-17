#pragma once 

#include "../include/Fruit.h"
#include "../include/NvFlex.h"

class FruitNvFlex : public Fruit {

public:

	~FruitNvFlex() {}

	//particles
	void SetParticles(FruitSolver solver, FruitBuffer buffer, int n) override {
		NvFlexSetParticles(solver.GetSolver(), buffer.GetBuffer(), n);
	}
	void GetParticles(FruitSolver solver, FruitBuffer buffer, int n) override {
		NvFlexGetParticles(solver.GetSolver(), buffer.GetBuffer(), n);
	}

	//rest particles
	void SetRestParticles(FruitSolver solver, FruitBuffer buffer, int n) override {
		NvFlexSetRestParticles(solver.GetSolver(), buffer.GetBuffer(), n);
	};
	void GetRestParticles(FruitSolver solver, FruitBuffer buffer, int n) override {
		NvFlexGetRestParticles(solver.GetSolver(), buffer.GetBuffer(), n);
	};

	//velocities
	void SetVelocities(FruitSolver solver, FruitBuffer buffer, int n) override {
		NvFlexSetVelocities(solver.GetSolver(), buffer.GetBuffer(), n);
	}
	void GetVelocities(FruitSolver solver, FruitBuffer buffer, int n) override {
		NvFlexGetVelocities(solver.GetSolver(), buffer.GetBuffer(), n);
	}

	//phases
	void SetPhases(FruitSolver solver, FruitBuffer buffer, int n) override {
		NvFlexSetPhases(solver.GetSolver(), buffer.GetBuffer(), n);
	}
	void GetPhases(FruitSolver solver, FruitBuffer buffer, int n) override {
		NvFlexGetPhases(solver.GetSolver(), buffer.GetBuffer(), n);
	}

	//densities
	void SetDensities(FruitSolver solver, FruitBuffer p, int n) override {}

	void GetDensities(FruitSolver solver, FruitBuffer buffer, int n) override {
		NvFlexGetDensities(solver.GetSolver(), buffer.GetBuffer(), n);
	}

	//anisotropy
	void SetAnisotropy(FruitSolver solver, FruitBuffer anisotropy1, FruitBuffer anisotropy2, FruitBuffer anisotropy3) override {}

	void GetAnisotropy(FruitSolver solver, FruitBuffer anisotropy1, FruitBuffer anisotropy2, FruitBuffer anisotropy3) override {
		NvFlexGetAnisotropy(solver.GetSolver(), anisotropy1.GetBuffer(), anisotropy2.GetBuffer(), anisotropy3.GetBuffer());
	}

	//normals
	void SetNormals(FruitSolver solver, FruitBuffer normals, int n) override {
		NvFlexSetNormals(solver.GetSolver(), normals.GetBuffer(), n);
	}
	void GetNormals(FruitSolver solver, FruitBuffer normals, int n) override {
		NvFlexGetNormals(solver.GetSolver(), normals.GetBuffer(), n);
	}

	//smooth particles
	void SetSmoothParticles(FruitSolver solver, FruitBuffer particles, int numParticles) override {}

	void GetSmoothParticles(FruitSolver solver, FruitBuffer buffer, int n) override {
		NvFlexGetSmoothParticles(solver.GetSolver(), buffer.GetBuffer(), n);
	}

	//indices
	void SetActiveIndices(FruitSolver solver, FruitBuffer buffer, int n) override {
		NvFlexSetActive(solver.GetSolver(), buffer.GetBuffer(), n);
	}

	void GetActiveIndices(FruitSolver solver, FruitBuffer buffer, int num) override {}

	//shapes 
	void SetShapes(
		FruitSolver solver,
		FruitBuffer geometry,
		FruitBuffer positions,
		FruitBuffer rotations,
		FruitBuffer prevPositions,
		FruitBuffer prevRotations,
		FruitBuffer flags,
		int numShapes) override {

		NvFlexSetShapes(
			solver.GetSolver(),
			geometry.GetBuffer(),
			positions.GetBuffer(),
			rotations.GetBuffer(),
			prevPositions.GetBuffer(),
			prevRotations.GetBuffer(),
			flags.GetBuffer(),
			numShapes
		);
	}

	void GetShapes(
		FruitSolver solver, 
		FruitBuffer geometry, 
		FruitBuffer positions, 
		FruitBuffer rotations, 
		FruitBuffer prevPositions, 
		FruitBuffer prevRotations, 
		FruitBuffer flags, 
		int numShapes) override {}

	//rigids 
	void SetRigids(
		FruitSolver solver,
		FruitBuffer offsets,
		FruitBuffer indices,
		FruitBuffer localPositions,
		FruitBuffer localNormals,
		FruitBuffer coefficients,
		FruitBuffer rotations,
		FruitBuffer translations,
		int numRigids,
		int numIndices) override {
	
		NvFlexSetRigids(
			solver.GetSolver(),
			offsets.GetBuffer(),
			indices.GetBuffer(),
			localPositions.GetBuffer(),
			localNormals.GetBuffer(),
			coefficients.GetBuffer(),
			rotations.GetBuffer(),
			translations.GetBuffer(),
			numRigids,
			numIndices);
	};

	void GetRigidTransforms(FruitSolver solver, FruitBuffer rotations, FruitBuffer translations) override {
		NvFlexGetRigidTransforms(solver.GetSolver(), rotations.GetBuffer(), translations.GetBuffer());
	}

	// inflatables
	void SetInflatables(
		FruitSolver solver,
		FruitBuffer startTris,
		FruitBuffer numTris,
		FruitBuffer volumes,
		FruitBuffer pressures,
		FruitBuffer constraints,
		int numInflatables) override {
	
		NvFlexSetInflatables(
			solver.GetSolver(),
			startTris.GetBuffer(),
			numTris.GetBuffer(),
			volumes.GetBuffer(),
			pressures.GetBuffer(),
			constraints.GetBuffer(),
			numInflatables);
	};

	// springs
	void SetSprings(
		FruitSolver solver,
		FruitBuffer indices,
		FruitBuffer lengths,
		FruitBuffer stiffness,
		int numSprings) override {
	
		NvFlexSetSprings(
			solver.GetSolver(),
			indices.GetBuffer(),
			lengths.GetBuffer(),
			stiffness.GetBuffer(),
			numSprings);
	};

	//dynamic triangles
	void SetDynamicTriangles(FruitSolver solver, FruitBuffer indices, FruitBuffer normals, int numTris) override {
		NvFlexSetDynamicTriangles(solver.GetSolver(), indices.GetBuffer(), normals.GetBuffer(), numTris);
	};

	void GetDynamicTriangles(FruitSolver solver, FruitBuffer indices, FruitBuffer normals, int numTris) override {
		NvFlexGetDynamicTriangles(solver.GetSolver(), indices.GetBuffer(), normals.GetBuffer(), numTris);
	}
};