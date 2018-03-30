#include "cell.h"
#include <memory>

#include "fruit_extensions/NvFlexImplFruit.h"

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

	float radius = 0.08f;

	buffers.numExtraParticles = 20000;

	flexParams.params.numIterations = 3;
	flexParams.numSubsteps = 5;

	flexParams.params.radius = radius;
	flexParams.params.solidRestDistance = 0.075f;
	flexParams.params.fluidRestDistance = 0.05f;

	flexParams.params.dynamicFriction = 0.125f;
	flexParams.params.dissipation = 0.0f;
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

	// TODO: need to refactoring
	static int j = 0;
	
	static auto ligandGroup = new LigandGroup(&buffers, kernel.get());

	cytoplasm->Update();
	shell->Update();
	kernel->Update();

	static auto receptors = new Receptors(&buffers, shell.get(), ligandGroup);

	//receptors->Update();

	if (j < 400) {
		j++;
		return;
	}

	if (j == 400) {
		j++;
		std::clog << "Cytoskeleton create" << std::endl;
		cytoskeleton2 = new Cytoskeleton2(kernel.get(), shell.get());
		return;
	}

	cytoskeleton2->Update();

}

void Cell::Sync()
{
	//std::unique_ptr<Fruit> fruit(new FruitNvFlex());
}

void Cell::Draw() {
	shell->Draw();
	cytoplasm->Draw();
}