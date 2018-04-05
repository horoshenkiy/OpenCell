#ifndef CELL_H
#define CELL_H

#include "components\cytoplasm.h"
#include "components\kernel.h"
#include "components\shell.h"
#include "components\receptors.h"
#include "components\ligands.h"
#include "components\receptors.h"
#include <components\cytoskeleton.h>

#include <fruit/controller/render_controller/render_param.h>

class Cell {

public:

	Cell() = default;
	
	// need incapsulation
	std::unique_ptr<Cytoplasm> cytoplasm;
	std::unique_ptr<Kernel>	kernel;
	std::unique_ptr<Shell> shell;
	Cytoskeleton* cytoskeleton;
	LigandGroup* ligandGroup;
	Receptors* receptors;

	void Initialize();

	virtual void InitializeFromFile();

	virtual void PostInitialize();

	void clearBuffers();

	// update
	void Update();

	void Sync();

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

	int mNumFluidParticles = 0;

	Compute::FlexParams &flexParams = FlexParams::Get();
	Compute::SimBuffers &buffers = SimBuffers::Get();

	Render::RenderParam &renderParam = Render::RenderParam::Get();

};

#endif // CELL_H