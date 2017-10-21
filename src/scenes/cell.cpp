#include "cell.h"

// initialize
void Cell::Initialize(FlexController *flexController,
	SimBuffers *buffers,
	FlexParams *flexParams,
	RenderBuffers *renderBuffers,
	RenderParam *renderParam) {

	this->flexController = flexController;
	this->buffers = buffers;
	this->flexParams = flexParams;

	this->renderBuffers = renderBuffers;
	this->renderParam = renderParam;

	float minSize = 0.25f;
	float maxSize = 0.5f;
	float spacing = 4.0f;

	float radius = 0.1f;

	buffers->numExtraParticles = 20000;
	flexParams->numSubsteps = 3;

	flexParams->params.solidRestDistance = 0.070;

	flexParams->params.radius = radius;
	flexParams->params.dynamicFriction = 0.125f;
	flexParams->params.dissipation = 0.0f;
	flexParams->params.numIterations = 5;
	flexParams->params.particleCollisionMargin = flexParams->params.radius*0.05f;
	flexParams->params.relaxationFactor = 1.0f;
	flexParams->params.drag = 0.0f;
	flexParams->params.anisotropyScale = 25.0f;
	flexParams->params.smoothing = 1.f;
	flexParams->params.maxSpeed = 0.5f * flexParams->numSubsteps * radius * 60.0;
	flexParams->params.gravity[1] *= 1.0f;
	flexParams->params.collisionDistance = 0.040f;
	flexParams->params.solidPressure = 0.0f;

	flexParams->params.fluid = true;

	flexParams->params.fluidRestDistance = radius*0.65f;
	flexParams->params.viscosity = 0.0;
	flexParams->params.adhesion = 0.0f;
	flexParams->params.cohesion = 0.02f;

	clearBuffers();

	shell = new Shell(group++);
	shell->Initialize(buffers);

	receptors = new Receptors(group++);
	receptors->Initialize(buffers, shell);

	kernel = new Kernel(group++);
	kernel->Initialize(buffers, renderBuffers);

	cytoplasm = new Cytoplasm(group++);
	cytoplasm->Initialize(flexParams, buffers);

	mNumFluidParticles = cytoplasm->GetNumberOfParticles();

	renderParam->drawCloth = false;
	flexParams->warmup = true;

	renderParam->drawMesh = true;

}

// destroy
Cell::~Cell() {
	delete shell;
}

void Cell::clearBuffers() {
	buffers->triangles.resize(0);
	buffers->springIndices.resize(0);
	buffers->springStiffness.resize(0);
	buffers->springLengths.resize(0);
}

// main update
void Cell::Update() {

	// need to refactoring
	static int j = 0;

	cytoplasm->Update();
	shell->Update();
	kernel->Update();

	if (j < 400) {
		j++;
		return;
	}

	if (j == 400) {
		Vec3 startGrow = Vec3(0.4, -0.1, 0.0);
		startGrow += kernel->GetPositionCenter();

		cytoskeleton = new Cytoskeleton(buffers, group++, startGrow);
		cytoskeleton->Initialize(this, buffers);

		j++;
		return;
	}

	cytoskeleton->Update(kernel->GetRateCenter());
}

void Cell::Sync()
{
	Fruit *fruit = new FruitNvFlex();

	FruitSolver fruitSolver;
	fruitSolver.SetSolver(flexController->GetSolver());

	// send new particle data to the GPU
	fruit->SetRestParticles(fruitSolver, buffers->restPositions.GetBuffer(), buffers->restPositions.size());

	// update solver
	fruit->SetSprings(
		fruitSolver,
		buffers->springIndices.GetBuffer(),
		buffers->springLengths.GetBuffer(),
		buffers->springStiffness.GetBuffer(),
		buffers->springLengths.size());

	fruit->SetDynamicTriangles(
		fruitSolver,
		buffers->triangles.GetBuffer(),
		buffers->triangleNormals.GetBuffer(),
		buffers->triangles.size() / 3);
}

void Cell::Draw() {
	shell->Draw();
	cytoplasm->Draw();
}