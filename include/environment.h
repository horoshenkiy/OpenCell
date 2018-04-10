#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <fruit/controller/compute_controller/sim_buffers.h>

using namespace FruitWork;

class Environment {

public:

	Environment() : buffers(Compute::SimBuffers::Get()) {}

	LigandGroup* CreateLigandGroup(Vec3 position) {
		ligandGroup = new LigandGroup(position);
		return ligandGroup;
	}

	LigandGroup* GetLigandGroup() const {
		return ligandGroup;
	}

	~Environment() {
		delete ligandGroup;
	}

private:

	// TODO: make shared_ptr
	LigandGroup* ligandGroup;

	Compute::SimBuffers &buffers;
};

#endif // ENVIRONMENT_H