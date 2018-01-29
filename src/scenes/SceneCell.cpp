#include "SceneCell.h"

#include "cell.h"

SceneCell::SceneCell(const char* name, 
	FlexController* flexController, 
	FlexParams* flexParams) : Scene(name) {
	
	this->mName = name;
	this->flexController = flexController;
	this->flexParams = flexParams;
}

void SceneCell::Initialize(FlexController *flexController, FlexParams *flexParams) {

	this->flexController = flexController;
	this->flexParams = flexParams;

	cell = new Cell();
	cell->Initialize();
}

void SceneCell::InitializeFromFile(FlexController *flexController, FlexParams *flexParams) {

	this->flexController = flexController;
	this->flexParams = flexParams;
}

void SceneCell::PostInitialize() {
	this->cell->PostInitialize();

	// calculate particle bounds
	Vec3 particleLower, particleUpper;
	GetParticleBounds(&buffers, particleLower, particleUpper);

	// accommodate shapes
	Vec3 shapeLower, shapeUpper;
	GetShapeBounds(&buffers, shapeLower, shapeUpper);

	// update bounds
	sceneLower = Min(Min(sceneLower, particleLower), shapeLower) - Vec3(flexParams->params.collisionDistance);
	sceneUpper = Max(Max(sceneUpper, particleUpper), shapeUpper) + Vec3(flexParams->params.collisionDistance);
}

// destroy
//////////////////////////////////////////////
SceneCell::~SceneCell() {
	delete cell;
}

void SceneCell::clearBuffers() {
	cell->clearBuffers();
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