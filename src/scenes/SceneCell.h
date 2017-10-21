#pragma once

#include <vector>
#include "../scenes.h"
#include "cell.h"

class SceneCell : public Scene {

private:
	Cell cell;

public:

	// constructors and initialize
	//////////////////////////////////////////////////////
	SceneCell() {}
	SceneCell(const char* name) : Scene(name) {}

	void Initialize(FlexController *flexController,
		SimBuffers *buffers,
		FlexParams *flexParams,
		RenderBuffers *renderBuffers,
		RenderParam *renderParam);

	// destroy
	//////////////////////////////////////////////
	~SceneCell() {
		//delete cell;
	}

	void clearBuffers() {
		cell.clearBuffers();
	}

	// update
	/////////////////////////////////////////////////////
	void Sync() {
		cell.Sync();
	}

	void Update() {
		Vec3 centerScene = cell.kernel->GetPositionCenter();

		sceneLower.x = centerScene.x - 2.0;
		sceneLower.z = centerScene.z - 2.0;

		sceneUpper.x = centerScene.x + 2.0;
		sceneUpper.z = centerScene.z + 2.0;

		cell.Update();
	}

	void Draw() {
		cell.Draw();
	}

};