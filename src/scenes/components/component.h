#pragma once

#include "../../controller/compute_controller/SimBuffers.h"
#include "../../controller/render_controller/RenderBuffer.h"

class Component {
protected:
	SimBuffers *buffers;
	RenderBuffers *renderBuffers;

public:

	virtual void Initialize() {}
	virtual void PostInitialize() {}

	virtual void Update() {}

	virtual void Sync() {}

	virtual void Draw() {}
};