#pragma once

#include "components\cytoplasm.h"
#include "components\kernel.h"
#include "components\shell.h"
#include "components\Receptors.h"
#include "components\Ligands.h"
#include "components\Receptors.h"

#include "biology_object.h"

class Serializer;

class Cytoskeleton2;

class Cell : public BiologyObject {

public:
	
	// need incapsulation
	std::unique_ptr<Cytoplasm> cytoplasm;
	std::unique_ptr<Kernel>	kernel;
	std::unique_ptr<Shell> shell;
	Cytoskeleton2* cytoskeleton2;
	LigandGroup* ligandGroup;
	Receptors* receptors;

	// constructors and initialize
	Cell() : BiologyObject() {}

	void Initialize() override;

	virtual void InitializeFromFile() override;

	virtual void PostInitialize() override;

	void clearBuffers();

	// update
	void Update() override;

	void Sync() override;

	void Draw();

	//////////////////////////////////////////////////////
	template<class Archive>
	void save(Archive &archive) const {
		archive(group, mNumFluidParticles);
		archive(*cytoplasm, *kernel, *shell);
	}

	template<class Archive>
	void load(Archive &archive) {
		archive(group, mNumFluidParticles);

		if (cytoplasm == nullptr)
			cytoplasm = std::make_unique<Cytoplasm>();

		if (kernel == nullptr)
			kernel = std::make_unique<Kernel>();

		if (shell == nullptr)
			shell = std::make_unique<Shell>();

		archive(*cytoplasm, *kernel, *shell);
	}

private:

	friend bool operator==(const Cell &lCell, const Cell &rCell);

	int group = 0;

	int mNumFluidParticles = 0;
};

#include <fruit/utilits/Serializer.h>

#include "components\cytoskeleton2.h"
