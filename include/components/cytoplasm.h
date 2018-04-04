#ifndef CYTOPLASM_H
#define CYTOPLASM_H

#include <fruit/controller/compute_controller/flex_params.h>
#include "component.h"

class Cytoplasm : public Component {

public:

	// constructors and initialize
	Cytoplasm();

	void Initialize() override;

	// getters and setters
	int GetNumberOfParticles() const { return numberOfParticles; }

	template<class Archive>
	void serialize(Archive &archive) {
		archive(group, numberOfParticles);
	}

private:

	//int group = -1;
	int numberOfParticles = 0;
};

#endif // CYTOPLASM_H