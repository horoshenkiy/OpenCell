#include "ComputeController.h"

void ComputeController::Initialize(FlexController *flexController, 
								  FlexParams *flexParams, 
								  SimBuffers *buffers, 
								  RenderParam *renderParam,
								  Scene *scene) {
	
	this->flexController = flexController;
	this->flexParams = flexParams;
	this->buffers = buffers;

	this->renderParam = renderParam;

	this->scene = scene;
}