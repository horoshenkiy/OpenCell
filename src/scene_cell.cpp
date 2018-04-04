#include <scene_cell.h>
#include <cell.h>

void SceneCell::Initialize() {
	this->buffers = &SimBuffers::Get();
	this->flexParams = &Compute::FlexParams::Get();

	cell = new Cell();
	cell->Initialize();
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

	delete cell;
}

void SceneCell::ClearBuffers() {
	cell->clearBuffers();
}

SceneCell::~SceneCell() {
	delete cell;
}

// update
/////////////////////////////////////////////////////
void SceneCell::Sync() {
	cell->Sync();
}

void SceneCell::Update() {
	Vec3 centerScene = cell->kernel->GetPositionCenter();

	sceneLower.x = centerScene.x - 2.0f;
	sceneLower.z = centerScene.z - 2.0f;

	sceneUpper.x = centerScene.x + 2.0f;
	sceneUpper.z = centerScene.z + 2.0f;

	cell->Update();
}

void SceneCell::Draw() {
	cell->Draw();
}
