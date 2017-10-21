#include "SceneCell.h"

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

	cell.Initialize(flexController, buffers, flexParams, renderBuffers, renderParam);
}