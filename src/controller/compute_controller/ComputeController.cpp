#include "ComputeController.h"

void ComputeController::Initialize(FlexController *flexController, 
								  FlexParams *flexParams, 
								  RenderParam *renderParam,
								  Scene *scene) {
	
	this->flexController = flexController;
	this->flexParams = flexParams;
	this->buffers = &SimBuffers::Get();

	this->renderParam = renderParam;

	this->scene = scene;
}

int ComputeController::GetActiveCount() const {
	return NvFlexGetActiveCount(flexController->GetSolver());
}

int ComputeController::GetDiffuseParticles() const {
	return NvFlexGetDiffuseParticles(flexController->GetSolver(), nullptr, nullptr, nullptr);
}

void ComputeController::Update() {
	
	// send any particle updates to the solver
	FruitSolver fruitSolver;
	fruitSolver.SetSolver(flexController->GetSolver());

	fruit->SetParticles(fruitSolver, buffers->positions.GetBuffer(), buffers->positions.size());
	fruit->SetRestParticles(fruitSolver, buffers->restPositions.GetBuffer(), buffers->restPositions.size());
	fruit->SetVelocities(fruitSolver, buffers->velocities.GetBuffer(), buffers->velocities.size());
	fruit->SetPhases(fruitSolver, buffers->phases.GetBuffer(), buffers->phases.size());
	fruit->SetActiveIndices(fruitSolver, buffers->activeIndices.GetBuffer(), buffers->activeIndices.size());

	// allow scene to update constraints etc
	scene->Sync();

	if (flexParams->shapesChanged)
	{
		fruit->SetShapes(
			fruitSolver,
			buffers->shapeGeometry.GetBuffer(),
			buffers->shapePositions.GetBuffer(),
			buffers->shapeRotations.GetBuffer(),
			buffers->shapePrevPositions.GetBuffer(),
			buffers->shapePrevRotations.GetBuffer(),
			buffers->shapeFlags.GetBuffer(),
			int(buffers->shapeFlags.size()));

		flexParams->shapesChanged = false;
	}


	// rigids
	if (buffers->rigidOffsets.size()) {
		fruit->SetRigids(
			fruitSolver,
			buffers->rigidOffsets.GetBuffer(),
			buffers->rigidIndices.GetBuffer(),
			buffers->rigidLocalPositions.GetBuffer(),
			buffers->rigidLocalNormals.GetBuffer(),
			buffers->rigidCoefficients.GetBuffer(),
			buffers->rigidRotations.GetBuffer(),
			buffers->rigidTranslations.GetBuffer(),
			buffers->rigidOffsets.size() - 1,
			buffers->rigidIndices.size());
	}

	fruit->SetShapes(
		fruitSolver,
		buffers->shapeGeometry.GetBuffer(),
		buffers->shapePositions.GetBuffer(),
		buffers->shapeRotations.GetBuffer(),
		buffers->shapePrevPositions.GetBuffer(),
		buffers->shapePrevRotations.GetBuffer(),
		buffers->shapeFlags.GetBuffer(),
		int(buffers->shapeFlags.size()));

	if (!g_pause || g_step)
	{
		// tick solver
		NvFlexSetParams(flexController->GetSolver(), &flexParams->params);
		NvFlexUpdateSolver(flexController->GetSolver(), timer.dt, flexParams->numSubsteps, flexParams->profile);

		g_frame++;
		g_step = false;
	}

	// read back base particle data
	// Note that flexGet calls don't wait for the GPU, they just queue a GPU copy 
	// to be executed later.
	// When we're ready to read the fetched buffers we'll Map them, and that's when
	// the CPU will wait for the GPU flex update and GPU copy to finish.

	fruit->GetParticles(fruitSolver, buffers->positions.GetBuffer(), buffers->positions.size());
	fruit->GetVelocities(fruitSolver, buffers->velocities.GetBuffer(), buffers->velocities.size());
	fruit->GetNormals(fruitSolver, buffers->normals.GetBuffer(), buffers->normals.size());

	// readback triangle normals
	if (buffers->triangles.size()) {
		fruit->GetDynamicTriangles(
			fruitSolver,
			buffers->triangles.GetBuffer(),
			buffers->triangleNormals.GetBuffer(),
			buffers->triangles.size() / 3);
	}

	// readback rigid transforms
	if (buffers->rigidOffsets.size()) {
		fruit->GetRigidTransforms(fruitSolver, buffers->rigidRotations.GetBuffer(), buffers->rigidTranslations.GetBuffer());
	}

	if (!flexParams->interop)
	{
		// if not using interop then we read back fluid data to host
		if (renderParam->drawEllipsoids) {
			fruit->GetSmoothParticles(fruitSolver, buffers->smoothPositions.GetBuffer(), buffers->smoothPositions.size());
			fruit->GetAnisotropy(fruitSolver,
				buffers->anisotropy1.GetBuffer(),
				buffers->anisotropy2.GetBuffer(),
				buffers->anisotropy3.GetBuffer());
		}

		// read back diffuse data to host
		if (renderParam->drawDensity)
			fruit->GetDensities(fruitSolver, buffers->densities.GetBuffer(), buffers->positions.size());
	}
}