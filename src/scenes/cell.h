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

	// destructors (need add delete)
	~Cell();

	void clearBuffers();

	// update
	void Update() override;

	void Sync() override;

	void Draw();

private:

	friend Serializer;
	friend bool operator==(const Cell &lCell, const Cell &rCell);

	int group = 0;

	int mNumFluidParticles = 0;
};

#include "../Serializer.h"
