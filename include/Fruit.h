#pragma once

#include "FruitExt.h"
#include "FruitExtTypes.h"

// write documentation of this class
// Loader data to GPU
class Fruit {

public:

	// get count particles 
	//virtual int GetActiveCount() = 0;

	// load params
	//virtual void SetParams(FruitSolver solver, FruitParams params) = 0;

	// functions for particles 
	virtual void SetParticles() = 0;
	virtual void GetParticles() = 0;

	virtual void SetRestParticles() = 0;
	virtual void GetRestParticles() = 0;

	virtual void SetVelocities() = 0;
	virtual void GetVelocities() = 0;

	virtual void SetPhases() = 0;
	virtual void GetPhases() = 0;

	virtual void GetDensities() = 0;

	virtual void SetActiveIndices() = 0;

	// functions for data of graphics
	virtual void GetAnisotropy() = 0;

	virtual void SetNormals() = 0;
	virtual void GetNormals() = 0;

	// functions for smooth and diffuse particles
	virtual void GetSmoothParticles() = 0;

	//virtual void SetDiffusePositions(FruitSolver solver, FruitBuffer buffer, int num) = 0;
	//virtual void GetDiffusePositions(FruitSolver solver, FruitBuffer buffer, int num) = 0;

	//virtual void SetDiffuseVelocities(FruitSolver solver, FruitBuffer buffer, int num) = 0;
	//virtual void GetDiffuseVelocities(FruitSolver solver, FruitBuffer buffer, int num) = 0;

	//virtual void SetDiffuseIndices(FruitSolver solver, FruitBuffer buffer, int num) = 0;
	//virtual void GetDiffuseIndices(FruitSolver solver, FruitBuffer buffer, int num) = 0;

	// function for shapes 
	virtual void SetShapes() = 0;

	// functions for rigids
	virtual void SetRigids() = 0;
	virtual void GetRigidTransforms() = 0;

	// functions for inflatables
	virtual void SetInflatables() = 0;

	// functions for springs
	virtual void SetSprings() = 0;

	// functions for dynamic triangles
	virtual void SetDynamicTriangles() = 0;
	virtual void GetDynamicTriangles() = 0;
};
