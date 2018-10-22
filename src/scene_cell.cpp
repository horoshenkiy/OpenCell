#include <scene_cell.h>
#include <cell.h>

void SceneCell::Initialize() {
	this->buffers = &SimBuffers::Get();
	this->flexParams = &Compute::FlexParams::Get();

	// creating of cell
	cell = std::make_unique<Cell>();
	cell->Initialize();

	// creating of environment
	environment = std::make_unique<Environment>();
	environment->CreateLigandGroup(cell->GetPositionCenter());
}

void SceneCell::InitializeFromFile() {
	this->buffers = &SimBuffers::Get();
	this->flexParams = &Compute::FlexParams::Get();
}

void SceneCell::PostInitialize() {
	this->cell->PostInitialize();

	// calculate particle bounds
	Vec3 particleLower, particleUpper;
	GetParticleBounds(buffers, particleLower, particleUpper);

	// accommodate shapes
	Vec3 shapeLower, shapeUpper;
	GetShapeBounds(buffers, shapeLower, shapeUpper);

	// update bounds
	sceneLower = Min(Min(sceneLower, particleLower), shapeLower) - Vec3(flexParams->params.collisionDistance);
	sceneUpper = Max(Max(sceneUpper, particleUpper), shapeUpper) + Vec3(flexParams->params.collisionDistance);
}

// destroy
//////////////////////////////////////////////
void SceneCell::Reset() {
	sceneLower = FLT_MAX;
	sceneUpper = -FLT_MAX;

	environment.reset(nullptr);
	cell.reset(nullptr);
}

// update
/////////////////////////////////////////////////////

void SceneCell::Update() {
	Vec3 centerScene = cell->GetPositionCenter();
	sceneLower = { centerScene.x - 2.0f, 0, centerScene.z - 2.0f };
	sceneUpper = { centerScene.x + 2.0f, 0, centerScene.z + 2.0f };
	
	cell->Update(environment->GetLigandGroup());
}


