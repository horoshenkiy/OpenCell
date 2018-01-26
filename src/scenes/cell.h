#pragma once

#include "components\cytoplasm.h"
#include "components\kernel.h"
#include "components\shell.h"
#include "components\Cytoskeleton.h"
#include "components\Receptors.h"

#include "BiologyObject.h"

class Serializer;

class Cell : public BiologyObject {

public:
	
	// need incapsulation
	Cytoplasm*  cytoplasm = nullptr;
	Kernel*		kernel = nullptr;
	Shell*		shell = nullptr;
	Cytoskeleton* cytoskeleton = nullptr;
	//Receptors*	receptors;

	// constructors and initialize
	Cell() = default;

	Cell(SimBuffers* buffers, RenderBuffers* renderBuffers) {
		this->buffers = buffers;
		this->renderBuffers = renderBuffers;
	}

	void Initialize(FlexController *flexController,
		SimBuffers *buffers,
		FlexParams *flexParams,
		RenderBuffers *renderBuffers,
		RenderParam *renderParam) override;

	virtual void InitializeFromFile(FlexController *flexController,
									SimBuffers *buffers,
									FlexParams *flexParams,
									RenderBuffers *renderBuffers,
									RenderParam *renderParam) override;

	virtual void PostInitialize() override;

	// destructors (need add delete)
	~Cell();

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
			cytoplasm = new Cytoplasm(buffers);

		if (kernel == nullptr)
			kernel = new Kernel(buffers, renderBuffers);

		if (shell == nullptr)
			shell = new Shell(buffers);

		archive(*cytoplasm, *kernel, *shell);
	}

private:

	friend bool operator==(const Cell &lCell, const Cell &rCell);

	int group = 0;

	int mNumFluidParticles = 0;
};

#include "../Serializer.h"
