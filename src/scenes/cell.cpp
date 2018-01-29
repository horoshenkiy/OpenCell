#include "cell.h"
#include <memory>

// initialize
void Cell::Initialize() {

	float minSize = 0.25f;
	float maxSize = 0.5f;
	float spacing = 4.0f;

	float radius = 0.1f;

	buffers.numExtraParticles = 20000;
	flexParams.numSubsteps = 3;

	flexParams.params.solidRestDistance = 0.070f;

	flexParams.params.radius = radius;
	flexParams.params.dynamicFriction = 0.125f;
	flexParams.params.dissipation = 0.0f;
	flexParams.params.numIterations = 5;
	flexParams.params.particleCollisionMargin = flexParams.params.radius*0.05f;
	flexParams.params.relaxationFactor = 1.0f;
	flexParams.params.drag = 0.0f;
	flexParams.params.anisotropyScale = 25.0f;
	flexParams.params.smoothing = 1.f;
	flexParams.params.maxSpeed = 0.5f * flexParams.numSubsteps * radius * 60.0f;
	flexParams.params.gravity[1] *= 1.0f;
	flexParams.params.collisionDistance = 0.040f;
	flexParams.params.solidPressure = 0.0f;

	flexParams.params.fluid = true;

	flexParams.params.fluidRestDistance = radius*0.65f;
	flexParams.params.viscosity = 0.0;
	flexParams.params.adhesion = 0.0f;
	flexParams.params.cohesion = 0.02f;

	clearBuffers();

	shell = std::make_unique<Shell>();
	shell->Initialize();

	kernel = std::make_unique<Kernel>();
	kernel->Initialize();

	cytoplasm = std::make_unique<Cytoplasm>();
	cytoplasm->Initialize();

	mNumFluidParticles = cytoplasm->GetNumberOfParticles();

	renderParam.drawCloth = false;
	flexParams.warmup = true;

	renderParam.drawMesh = true;
}

void Cell::InitializeFromFile() {}

void Cell::PostInitialize() {
	float minSize = 0.25f;
	float maxSize = 0.5f;
	float spacing = 4.0f;

	float radius = 0.1f;

	buffers.numExtraParticles = 20000;
	flexParams.numSubsteps = 10;

	flexParams.params.solidRestDistance = 0.100f;

	flexParams.params.radius = radius;
	flexParams.params.dynamicFriction = 0.125f;
	flexParams.params.dissipation = 0.0f;
	flexParams.params.numIterations = 5;
	flexParams.params.particleCollisionMargin = flexParams.params.radius*0.05f;
	flexParams.params.relaxationFactor = 1.0f;
	flexParams.params.drag = 0.0f;
	flexParams.params.anisotropyScale = 25.0f;
	flexParams.params.smoothing = 1.f;
	flexParams.params.maxSpeed = 0.5f * flexParams.numSubsteps * radius * 60.0f;
	flexParams.params.gravity[1] *= 1.0f;
	flexParams.params.collisionDistance = 0.040f;
	flexParams.params.solidPressure = 0.0f;

	flexParams.params.fluid = true;

	flexParams.params.fluidRestDistance = radius*0.65f;
	flexParams.params.viscosity = 0.0;
	flexParams.params.adhesion = 0.0f;
	flexParams.params.cohesion = 0.02f;

	mNumFluidParticles = cytoplasm->GetNumberOfParticles();

	renderParam.drawCloth = false;
	flexParams.warmup = true;

	renderParam.drawMesh = true;
}

void Cell::clearBuffers() {
	buffers.triangles.resize(0);
	buffers.springIndices.resize(0);
	buffers.springStiffness.resize(0);
	buffers.springLengths.resize(0);
}

// main update
void Cell::Update() {

	// need to refactoring
	static int j = 1;

	cytoplasm->Update();
	shell->Update();
	kernel->Update();

	Vec3 startGrow = Vec3(0.4f, -0.1f, 0.0f);
	startGrow += kernel->GetPositionCenter();

	//CreateRigidCapsule(*buffers, *renderBuffers, 0.2f, 1.0f, 10, 20, startGrow, Vec3(0.096f), 0.0f, 0.0016f, Vec3(0.0f), 0.25f, group++);
//	j++;
	if (j % 100 == 0) {
		CreateRigidCapsule(buffers, renderBuffers, 0.1f, 1.0f, 10, 20, startGrow, Vec3(0.05f), 0.0f, 0.0016f, Vec3(0.0f), 0.05f, 5);
		//j++;

		return;
	}

	/*if (j == 400) {
		Vec3 startGrow = Vec3(0.4f, -0.1f, 0.0f);
		startGrow += kernel->GetPositionCenter();

		cytoskeleton = new Cytoskeleton(this, buffers, group++, startGrow);
		cytoskeleton->Initialize(this, buffers);

		j++;
		return;
	}

	cytoskeleton->Update(kernel->GetRateCenter());*/
}

void Cell::Sync()
{
	std::unique_ptr<Fruit> fruit(new FruitNvFlex());

	FruitSolver fruitSolver;
	fruitSolver.SetSolver(flexController.GetSolver());

	// send new particle data to the GPU
	fruit->SetRestParticles(fruitSolver, SimBuffers::Get().restPositions.GetBuffer(), SimBuffers::Get().restPositions.size());

	// update solver
	fruit->SetSprings(
		fruitSolver,
		buffers.springIndices.GetBuffer(),
		buffers.springLengths.GetBuffer(),
		buffers.springStiffness.GetBuffer(),
		buffers.springLengths.size());

	fruit->SetDynamicTriangles(
		fruitSolver,
		buffers.triangles.GetBuffer(),
		buffers.triangleNormals.GetBuffer(),
		buffers.triangles.size() / 3);
}

void Cell::Draw() {
	shell->Draw();
	cytoplasm->Draw();
}