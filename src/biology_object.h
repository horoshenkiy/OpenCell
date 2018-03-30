#ifndef BIOLOGY_OBJECT_H
#define BIOLOGY_OBJECT_H

#include <fruit/controller/compute_controller/flex_params.h>
#include <fruit/controller/compute_controller/flex_controller.h>
#include <fruit/controller/compute_controller/sim_buffers.h>
#include <fruit/controller/render_controller/render_buffer.h>
#include <fruit/controller/render_controller/render_param.h>

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

#endif // BIOLOGY_OBJECT_H