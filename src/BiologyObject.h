#pragma once

#include "../controller/compute_controller/FlexParams.h"
#include "../controller/compute_controller/FlexController.h"
#include "../controller/compute_controller/SimBuffers.h"

#include "../controller/render_controller/RenderBuffer.h"
#include "../controller/render_controller/RenderParam.h"

class BiologyObject
{
protected:
	FlexController &flexController;
	FlexParams &flexParams;
	SimBuffers &buffers;

	RenderBuffers &renderBuffers;
	RenderParam &renderParam;

public:

	BiologyObject() : 
		flexController(FlexController::Instance()),
		flexParams(FlexParams::Get()),
		buffers(SimBuffers::Get()), 
		renderBuffers(RenderBuffers::Get()),
		renderParam(RenderParam::Get()) {}

	virtual void Initialize() = 0;

	virtual void InitializeFromFile() = 0;

	virtual void PostInitialize() = 0;

	// update any buffers (all guaranteed to be mapped here)
	virtual void Update() {}

	// send any changes to flex (all buffers guaranteed to be unmapped here)
	virtual void Sync() {}

	virtual void Draw(int pass) {}
};