#pragma once

#include "../../controller/compute_controller/SimBuffers.h"
#include "../../controller/render_controller/RenderBuffer.h"

class Component {
protected:
	SimBuffers &buffers;
	RenderBuffers &renderBuffers;

	int group;

public:

	Component() : buffers(SimBuffers::Get()), renderBuffers(RenderBuffers::Get()) {
		static int count = 0;
		group = count++;
	}

	virtual ~Component() {}

	virtual void Initialize() {}

	virtual void Update() {}

	virtual void Sync() {}

	virtual void Draw() {}
};