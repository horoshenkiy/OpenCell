#pragma once

#include "component.h"
#include "../../controller/compute_controller/FlexParams.h"

class Serializer;

class Cytoplasm : public Component {

public:

	// constructors and initialize
	Cytoplasm();
	
	explicit Cytoplasm(SimBuffers *buffers) : Component(buffers, nullptr) {}
	
	explicit Cytoplasm(int group, SimBuffers *buffers);

	void Initialize(FlexParams *flexParams);

	// getters and setters
	int GetNumberOfParticles() const { return numberOfParticles; }

private:

	friend Serializer;
	friend bool operator==(const Cytoplasm &lCytoplasm, const Cytoplasm &rCytoplasm);

	int group = -1;
	int numberOfParticles = 0;
};