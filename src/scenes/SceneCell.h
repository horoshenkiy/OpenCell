#pragma once

#include "../scenes.h"

class Serializer;
class Cell;

class SceneCell : public Scene {

private:
	friend Serializer;
	friend bool operator==(const SceneCell &lScene, const SceneCell &rScene);

	Cell *cell = nullptr;

public:

	// constructors and initialize
	//////////////////////////////////////////////////////
	SceneCell() {}
	
	SceneCell(const char* name) : Scene(name) {}

	SceneCell(const char* name, 
		FlexController *flexController,
		SimBuffers *buffers,
		FlexParams *flexParams,
		RenderBuffers *renderBuffers,
		RenderParam *renderParam);

	void Initialize(FlexController *flexController,
		SimBuffers *buffers,
		FlexParams *flexParams,
		RenderBuffers *renderBuffers,
		RenderParam *renderParam) override;

	// destroy
	//////////////////////////////////////////////
	~SceneCell();

	void clearBuffers();

	// update
	/////////////////////////////////////////////////////
	void Sync() override;

	void Update() override;

	void Draw() override;

};

#include "../Serializer.h"

