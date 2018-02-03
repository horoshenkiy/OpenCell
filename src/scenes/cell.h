#pragma once

#include "components\cytoplasm.h"
#include "components\kernel.h"
#include "components\shell.h"
#include "components\Cytoskeleton.h"
#include "components\cytoskeleton2.h"
#include "components\Receptors.h"

#include "BiologyObject.h"

class Cell : public BiologyObject {

private:
	int group = 0;

	int mNumFluidParticles;

public:
	
	// need incapsulation
	Cytoplasm*  cytoplasm;
	Kernel*		kernel;
	Shell*		shell;
	Cytoskeleton* cytoskeleton;
	Cytoskeleton2* cytoskeleton2;
	Receptors*	receptors;

	// constructors and initialize
	Cell() {}

	void Initialize(FlexController *flexController,
					SimBuffers *buffers,
					FlexParams *flexParams,
					RenderBuffers *renderBuffers,
					RenderParam *renderParam);

	// destructors (need add delete)
	~Cell();

	void clearBuffers();

	// update
	void Update();

	void Sync();

	void Draw();
};
