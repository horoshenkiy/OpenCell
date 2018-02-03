#pragma once

#include "../../controller/compute_controller/SimBuffers.h"
#include "../../controller/render_controller/RenderBuffer.h"

class Component {
protected:
	SimBuffers *buffers;
	RenderBuffers *renderBuffers;

public:

	Component() = default;

	Component(SimBuffers *buffers, RenderBuffers *renderBuffers) {
		this->buffers = buffers; 
		this->renderBuffers = renderBuffers;
	}

	virtual ~Component() {}

	virtual void Initialize() {}

	virtual void Update() {}

	virtual void Sync() {}

	virtual void Draw() {}
};