#ifndef COMPONENT_H
#define COMPONENT_H

#include "fruit/controller/compute_controller/sim_buffers.h"
#include "fruit/controller/render_controller/render_buffer.h"

using namespace FruitWork;

class Component {
public:

	Component() : 
		buffers(Compute::SimBuffers::Get()), 
		renderBuffers(Render::RenderBuffers::Get()) 
	{
		static int count = 0;
		group = count++;
	}

	virtual ~Component() {}

	virtual void Initialize() {}

	virtual void Update() {}

	virtual void Sync() {}

	virtual void Draw() {}

protected:
	Compute::SimBuffers &buffers;
	Render::RenderBuffers &renderBuffers;

	int group;

};

#endif // COMPONENT_H