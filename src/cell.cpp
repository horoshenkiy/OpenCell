#include <cell.h>
#include <memory>

// initialize
void Cell::Initialize() {

	float minSize = 0.25f;
	float maxSize = 0.5f;
	float spacing = 4.0f;

	float radius = 0.08f;

	buffers.numExtraParticles = 20000;
	
	flexParams.numSubsteps = 3;
	flexParams.params.numIterations = 5;

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

	// creating of shell
	shell = std::make_unique<Shell>();
	shell->Initialize();

	// creating of kernel
	kernel = std::make_unique<Kernel>();
	kernel->Initialize();

	//creating of cytoplasm
	cytoplasm = std::make_unique<Cytoplasm>();
	cytoplasm->Initialize();

	// creating of receptors
	receptors = std::make_unique<Receptors>(shell.get());
	cytoskeleton = std::make_unique<Cytoskeleton>(kernel.get(), shell.get());

	mNumFluidParticles = cytoplasm->GetNumberOfParticles();

	renderParam.drawCloth = false;
	flexParams.warmup = true;

	renderParam.drawMesh = true;
}

void Cell::InitializeFromFile() {}

void Cell::PostInitialize() {}

// main update
void Cell::Update(LigandGroup *ligandGroup) {
	// updating all components of cell
	cytoplasm->Update();
	shell->Update();
	kernel->Update();
	receptors->Update(ligandGroup);
	cytoskeleton->Update();
}

Vec3 Cell::GetPositionCenter() const {
	return kernel->GetPositionCenter();
}