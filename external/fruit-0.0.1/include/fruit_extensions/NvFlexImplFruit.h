#ifndef NV_FLEX_IMPL_FRUIT_H
#define NV_FLEX_IMPL_FRUIT_H

#include <fruit/fruit.h>
#include <flex/NvFlex.h>

#include <fruit/abst_compute_controller.h>

#include <fruit/controller/compute_controller/flex_controller.h>
#include <fruit/controller/compute_controller/flex_params.h>
#include <fruit/controller/compute_controller/sim_buffers.h>

#include <fruit/utilits/timer.h>
#include <fruit/application.h>

class FruitNvFlex : public AbstComputeController {

private:

	FlexController *flexController;
	FlexParams *flexParams;
	SimBuffers *buffers;

public:

	void Initialize(bool debug) override {
		RandInit();

		// init flex
		flexController = &FlexController::Instance();

		// initialize params
		flexParams = &FlexParams::Instance();

		// init benchmark 
		if (debug)
			std::cout << "Compute Device: " << flexController->GetDeviceName() << std::endl;

		// create compute buffer
		buffers = &SimBuffers::Instance(flexController->GetLib());
	}

	void PostInitialize() override {
		buffers->PostInitialize();
		buffers->BuildConstraints();
	}

	void InitializeGPU() override {
		// 	// main create method for the Flex solver
		NvFlexSolver *solver = NvFlexCreateSolver(
			flexController->GetLib(),
			buffers->maxParticles,
			buffers->maxDiffuseParticles,
			flexParams->maxNeighborsPerParticle
		);

		flexController->SetSolver(solver);

		// params of Flex
		NvFlexSetParams(solver, &(flexParams->params));

		SendBuffers();
	}

	void MapBuffers() override {
		buffers->MapBuffers();
	}

	void UnmapBuffers() override {
		buffers->UnmapBuffers();
	}

	int GetActiveCount() const override {
		return NvFlexGetActiveCount(flexController->GetSolver());
	}

	int GetDiffuseParticles() const override {
		return NvFlexGetDiffuseParticles(flexController->GetSolver(), nullptr, nullptr, nullptr);
	}

	void Update() {
		
		SetParticles();
		SetRestParticles();
		SetVelocities();
		SetPhases();
		SetActiveIndices();

		if (flexParams->shapesChanged) {
			SetShapes();
			flexParams->shapesChanged = false;
		}

		// rigids
		if (buffers->rigidOffsets.size())
			SetRigids();

		SetShapes();

		if (!FruitWork::AppParams::g_pause || FruitWork::AppParams::g_step)
		{
			// tick solver
			NvFlexSetParams(flexController->GetSolver(), &flexParams->params);
			NvFlexUpdateSolver(flexController->GetSolver(), FruitWork::Application::timer.dt, flexParams->numSubsteps, flexParams->profile);

			FruitWork::AppParams::g_frame++;
			FruitWork::AppParams::g_step = false;
		}

		// read back base particle data
		// Note that flexGet calls don't wait for the GPU, they just queue a GPU copy 
		// to be executed later.
		// When we're ready to read the fetched buffers we'll Map them, and that's when
		// the CPU will wait for the GPU flex update and GPU copy to finish.

		GetParticles();
		GetVelocities();
		GetNormals();

		// readback triangle normals
		if (buffers->triangles.size()) {
			GetDynamicTriangles();
		}

		// readback rigid transforms
		if (buffers->rigidOffsets.size()) {
			GetRigidTransforms();
		}
	}

	void SendBuffers() {

		SetParticles();
		SetRestParticles();
		SetVelocities();
		SetPhases();
		SetNormals();
		SetActiveIndices();

		// springs
		if (buffers->springIndices.size()) {
			assert((buffers->springIndices.size() & 1) == 0);
			assert((buffers->springIndices.size() / 2) == buffers->springLengths.size());

			SetSprings();
		}

		// rigids
		if (buffers->rigidOffsets.size()) 
			SetRigids();

		// inflatables
		if (buffers->inflatableTriOffsets.size()) 
			SetInflatables();

		// dynamic triangles 
		if (buffers->triangles.size()) 
			SetDynamicTriangles();

		// collision shapes
		if (buffers->shapeFlags.size()) 
			SetShapes();
	}

	void Sync() {
		// send new particle data to the GPU
		SetRestParticles();

		// update solver
		SetSprings();
		SetDynamicTriangles();
	}

	//particles
	void SetParticles() override {
		NvFlexSetParticles(
			flexController->GetSolver(), 
			buffers->positions.GetNvBuffer(), 
			buffers->positions.size()
		);
	}
	void GetParticles() override {
		NvFlexGetParticles(
			flexController->GetSolver(), 
			buffers->positions.GetNvBuffer(), 
			buffers->positions.size()
		);
	}

	//rest particles
	void SetRestParticles() override {
		NvFlexSetRestParticles(
			flexController->GetSolver(), 
			buffers->restPositions.GetNvBuffer(), 
			buffers->restPositions.size()
		);
	};
	void GetRestParticles() override {
		NvFlexGetRestParticles(
			flexController->GetSolver(),
			buffers->restPositions.GetNvBuffer(), 
			buffers->restPositions.size()
		);
	};

	//velocities
	void SetVelocities() override {
		NvFlexSetVelocities(
			flexController->GetSolver(),
			buffers->velocities.GetNvBuffer(),
			buffers->velocities.size()
		);
	}
	void GetVelocities() override {
		NvFlexGetVelocities(
			flexController->GetSolver(), 
			buffers->velocities.GetNvBuffer(), 
			buffers->velocities.size()
		);
	}

	//phases
	void SetPhases() override {
		NvFlexSetPhases(
			flexController->GetSolver(), 
			buffers->phases.GetNvBuffer(), 
			buffers->phases.size()
		);
	}
	void GetPhases() override {
		NvFlexGetPhases(
			flexController->GetSolver(), 
			buffers->phases.GetNvBuffer(), 
			buffers->phases.size()
		);
	}

	//densities
	void GetDensities() override {
		NvFlexGetDensities(
			flexController->GetSolver(),
			buffers->densities.GetNvBuffer(),
			buffers->densities.size()
		);
	}

	//anisotropy
	void GetAnisotropy() override {
		NvFlexGetAnisotropy(
			flexController->GetSolver(), 
			buffers->anisotropy1.GetNvBuffer(), 
			buffers->anisotropy2.GetNvBuffer(), 
			buffers->anisotropy3.GetNvBuffer()
		);
	}

	//normals
	void SetNormals() override {
		NvFlexSetNormals(
			flexController->GetSolver(), 
			buffers->normals.GetNvBuffer(), 
			buffers->normals.size()
		);
	}
	void GetNormals() override {
		NvFlexGetNormals(
			flexController->GetSolver(), 
			buffers->normals.GetNvBuffer(), 
			buffers->normals.size()
		);
	}

	//smooth particles
	void GetSmoothParticles() override {
		NvFlexGetSmoothParticles(
			flexController->GetSolver(), 
			buffers->smoothPositions.GetNvBuffer(), 
			buffers->smoothPositions.size()
		);
	}

	//indices
	void SetActiveIndices() override {
		NvFlexSetActive(
			flexController->GetSolver(), 
			buffers->activeIndices.GetNvBuffer(), 
			buffers->activeIndices.size()
		);
	}

	//shapes 
	void SetShapes() override {
		NvFlexSetShapes(
			flexController->GetSolver(),
			buffers->shapeGeometry.GetNvBuffer(),
			buffers->shapePositions.GetNvBuffer(),
			buffers->shapeRotations.GetNvBuffer(),
			buffers->shapePrevPositions.GetNvBuffer(),
			buffers->shapePrevRotations.GetNvBuffer(),
			buffers->shapeFlags.GetNvBuffer(),
			buffers->shapeFlags.size()
		);
	}

	//rigids 
	void SetRigids() override {
		NvFlexSetRigids(
			flexController->GetSolver(),
			buffers->rigidOffsets.GetNvBuffer(),
			buffers->rigidIndices.GetNvBuffer(),
			buffers->rigidLocalPositions.GetNvBuffer(),
			buffers->rigidLocalNormals.GetNvBuffer(),
			buffers->rigidCoefficients.GetNvBuffer(),
			buffers->rigidRotations.GetNvBuffer(),
			buffers->rigidTranslations.GetNvBuffer(),
			buffers->rigidOffsets.size() - 1,
			buffers->rigidIndices.size()
		);
	};

	void GetRigidTransforms() override {
		NvFlexGetRigidTransforms(
			flexController->GetSolver(), 
			buffers->rigidRotations.GetNvBuffer(), 
			buffers->rigidTranslations.GetNvBuffer()
		);
	}

	// inflatables
	void SetInflatables() override {
		NvFlexSetInflatables(
			flexController->GetSolver(),
			buffers->inflatableTriOffsets.GetNvBuffer(),
			buffers->inflatableTriCounts.GetNvBuffer(),
			buffers->inflatableVolumes.GetNvBuffer(),
			buffers->inflatablePressures.GetNvBuffer(),
			buffers->inflatableCoefficients.GetNvBuffer(),
			buffers->inflatableTriOffsets.size()
		);
	};

	// springs
	void SetSprings() override {
		NvFlexSetSprings(
			flexController->GetSolver(),
			buffers->springIndices.GetNvBuffer(),
			buffers->springLengths.GetNvBuffer(),
			buffers->springStiffness.GetNvBuffer(),
			buffers->springLengths.size()
		);
	};

	//dynamic triangles
	void SetDynamicTriangles() override {
		NvFlexSetDynamicTriangles(
			flexController->GetSolver(),
			buffers->triangles.GetNvBuffer(),
			buffers->triangleNormals.GetNvBuffer(),
			buffers->triangles.size() / 3
		);
	};
	void GetDynamicTriangles() override {
		NvFlexGetDynamicTriangles(
			flexController->GetSolver(), 
			buffers->triangles.GetNvBuffer(),
			buffers->triangleNormals.GetNvBuffer(),
			buffers->triangles.size() / 3
		);
	}
};

#endif // NV_FLEX_IMPL_FRUIT_H