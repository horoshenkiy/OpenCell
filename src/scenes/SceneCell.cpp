#include "SceneCell.h"

#include "cell.h"

SceneCell::SceneCell(const char* name, 
	FlexController* flexController, 
	FlexParams* flexParams, 
	RenderParam* renderParam) {
	
	this->mName = name;
	this->flexController = flexController;
	this->flexParams = flexParams;
	this->renderParam = renderParam;
}

void SceneCell::Initialize(FlexController *flexController,
	FlexParams *flexParams,
	RenderParam *renderParam) {

	this->flexController = flexController;
	this->buffers = &SimBuffers::Get();
	this->flexParams = flexParams;

	this->renderParam = renderParam;

	cell = new Cell();
	cell->Initialize(flexController, flexParams, renderParam);
}

void SceneCell::InitializeFromFile(FlexController *flexController,
	FlexParams *flexParams,
	RenderParam *renderParam) {

	this->flexController = flexController;
	this->buffers = buffers;
	this->flexParams = flexParams;

	this->renderParam = renderParam;
}

void SceneCell::PostInitialize() {
	this->cell->PostInitialize();
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