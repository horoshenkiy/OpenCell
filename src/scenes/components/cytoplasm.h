#pragma once

#include "component.h"
#include "../../controller/compute_controller/FlexParams.h"

class Serializer;

class Cytoplasm : public Component {

public:

	// constructors and initialize
	Cytoplasm();

	void Initialize(FlexParams *flexParams);

	// getters and setters
	int GetNumberOfParticles() const { return numberOfParticles; }

	template<class Archive>
	void serialize(Archive &archive) {
		archive(group, numberOfParticles);
	}

private:

	friend bool operator==(const Cytoplasm &lCytoplasm, const Cytoplasm &rCytoplasm);

	int group = -1;
	int numberOfParticles = 0;
};