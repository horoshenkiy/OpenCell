#include "SceneCell.h"

#include "cell.h"

SceneCell::SceneCell(const char* name, 
	FlexController* flexController, 
	SimBuffers* buffers, 
	FlexParams* flexParams, 
	RenderBuffers* renderBuffers, 
	RenderParam* renderParam) {
	
	this->mName = name;
	this->flexController = flexController;
	this->buffers = buffers;
	this->flexParams = flexParams;
	this->renderBuffers = renderBuffers;
	this->renderParam = renderParam;
}

void SceneCell::Initialize(FlexController *flexController,
	SimBuffers *buffers,
	FlexParams *flexParams,
	RenderBuffers *renderBuffers,
	RenderParam *renderParam) {

	this->flexController = flexController;
	this->buffers = buffers;
	this->flexParams = flexParams;

	this->renderBuffers = renderBuffers;
	this->renderParam = renderParam;

	cell = new Cell();
	cell->Initialize(flexController, buffers, flexParams, renderBuffers, renderParam);
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