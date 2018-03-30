#ifndef COMPONENT_H
#define COMPONENT_H

#include "fruit/controller/compute_controller/sim_buffers.h"
#include "fruit/controller/render_controller/render_buffer.h"

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

#endif // COMPONENT_H