#pragma once

#include "FruitExt.h"
#include "FruitExtTypes.h"

// write documentation of this class
class Fruit {

public:

	// functions for particles 
	virtual void SetParticles(FruitSolver solver, FruitBuffer p, int n) = 0;
	virtual void GetParticles(FruitSolver solver, FruitBuffer p, int n) = 0;

	virtual void SetRestParticles(FruitSolver solver, FruitBuffer p, int n) = 0;
	virtual void GetRestParticles(FruitSolver solver, FruitBuffer p, int n) = 0;

	virtual void SetVelocities(FruitSolver solver, FruitBuffer p, int n) = 0;
	virtual void GetVelocities(FruitSolver solver, FruitBuffer p, int n) = 0;

	virtual void SetPhases(FruitSolver solver, FruitBuffer p, int n) = 0;
	virtual void GetPhases(FruitSolver solver, FruitBuffer p, int n) = 0;

	virtual void SetDensities(FruitSolver solver, FruitBuffer p, int n) = 0;
	virtual void GetDensities(FruitSolver sovler, FruitBuffer p, int n) = 0;

	virtual void SetActiveIndices(FruitSolver solver, FruitBuffer buffer, int num) = 0;
	virtual void GetActiveIndices(FruitSolver solver, FruitBuffer buffer, int num) = 0;

	// functions for data of graphics
	virtual void SetAnisotropy(FruitSolver solver, FruitBuffer anisotropy1, FruitBuffer anisotropy2, FruitBuffer anisotropy3) = 0;
	virtual void GetAnisotropy(FruitSolver solver, FruitBuffer anisotropy1, FruitBuffer anisotropy2, FruitBuffer anisotropy3) = 0;

	virtual void SetNormals(FruitSolver solver, FruitBuffer normals, int n) = 0;
	virtual void GetNormals(FruitSolver solver, FruitBuffer normals, int n) = 0;

	// functions for smooth and diffuse particles
	virtual void SetSmoothParticles(FruitSolver solver, FruitBuffer particles, int numParticles) = 0;
	virtual void GetSmoothParticles(FruitSolver solver, FruitBuffer particles, int numParticles) = 0;

	//virtual void SetDiffusePositions(FruitSolver solver, FruitBuffer buffer, int num) = 0;
	//virtual void GetDiffusePositions(FruitSolver solver, FruitBuffer buffer, int num) = 0;

	//virtual void SetDiffuseVelocities(FruitSolver solver, FruitBuffer buffer, int num) = 0;
	//virtual void GetDiffuseVelocities(FruitSolver solver, FruitBuffer buffer, int num) = 0;

	//virtual void SetDiffuseIndices(FruitSolver solver, FruitBuffer buffer, int num) = 0;
	//virtual void GetDiffuseIndices(FruitSolver solver, FruitBuffer buffer, int num) = 0;

	// function for shapes 
	virtual void SetShapes(
		FruitSolver solver,
		FruitBuffer geometry,
		FruitBuffer positions,
		FruitBuffer rotations,
		FruitBuffer prevPositions,
		FruitBuffer prevRotations,
		FruitBuffer flags,
		int numShapes) = 0;
	//вряд ли пригодится
	virtual void GetShapes(
		FruitSolver solver,
		FruitBuffer geometry,
		FruitBuffer positions,
		FruitBuffer rotations,
		FruitBuffer prevPositions,
		FruitBuffer prevRotations,
		FruitBuffer flags,
		int numShapes) = 0;

	// functions for rigids
	virtual void SetRigids(
		FruitSolver solver,
		FruitBuffer offsets,
		FruitBuffer indices,
		FruitBuffer localPositions,
		FruitBuffer localNormals,
		FruitBuffer coefficients,
		FruitBuffer rotations,
		FruitBuffer translations,
		int numRigids,
		int numIndices) = 0;

	virtual void GetRigidTransforms(FruitSolver solver, FruitBuffer rotations, FruitBuffer translations) = 0;

	// functions for inflatables
	virtual void SetInflatables(
		FruitSolver solver,
		FruitBuffer startTris,
		FruitBuffer numTris,
		FruitBuffer volumes,
		FruitBuffer pressures,
		FruitBuffer constraints,
		int numInflatables) = 0;

	// functions for springs
	virtual void SetSprings(
		FruitSolver solver,
		FruitBuffer indices,
		FruitBuffer lengths,
		FruitBuffer stiffness,
		int numSprings) = 0;

	// functions for dynamic triangles
	virtual void SetDynamicTriangles(FruitSolver solver, FruitBuffer indices, FruitBuffer normals, int numTris) = 0;
	virtual void GetDynamicTriangles(FruitSolver solver, FruitBuffer indices, FruitBuffer normals, int numTris) = 0;
};
