#include "ComputeController.h"

void ComputeController::Initialize(FlexController *flexController, 
								  FlexParams *flexParams, 
								  RenderParam *renderParam,
								  Scene *scene) {
	
	this->flexController = flexController;
	this->flexParams = flexParams;
	this->buffers = &SimBuffers::Get();

	this->renderParam = renderParam;

	this->scene = scene;
}