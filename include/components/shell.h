#ifndef SHELL_H
#define SHELL_H

#include "component.h"
#include <random>

#include <fruit/primitives/primitive.h>

using namespace FruitWork;

// TODO: add constructor copy
class Shell : public Component {

public:

	//constructors and initialize

	Shell() : Component() {}

	void Initialize() override;

	// destructor
	~Shell() {
		if (cloth) delete cloth;
	}

	// getters and setters
	size_t GetIndBeginPosition() const { return cloth->GetIndBeginPos(); }

	size_t GetIndEndPosition() const { return cloth->GetIndEndPos(); }

	// update
	void Update() override;

	void Draw() override {}

	template<class Archive>
	void save(Archive &archive) const {
		archive(group);
	}

	template<class Archive>
	void load(Archive &archive) {
		archive(group);
	}

private:

	// cloth
	Primitives::Cloth *cloth = nullptr;
};

#endif // SHELL_H