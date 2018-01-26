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

	void InitializeFromFile(FlexController *flexController,
							SimBuffers *buffers,
							FlexParams *flexParams,
							RenderBuffers *renderBuffers,
							RenderParam *renderParam) override;

	void PostInitialize() override;

	// destroy
	//////////////////////////////////////////////
	~SceneCell();

	void clearBuffers();

	// update
	/////////////////////////////////////////////////////
	void Sync() override;

	void Update() override;

	void Draw() override;

	//////////////////////////////////////////////////////
	template<class Archive>
	void save(Archive &archive) const {
		archive(*cell, sceneLower, sceneUpper);
	}

	template<class Archive>
	void load(Archive &archive) {
		if (cell == nullptr) {
			cell = new Cell();
			cell->InitializeFromFile(flexController, buffers, flexParams, renderBuffers, renderParam);
		}

		archive(*cell, sceneLower, sceneUpper);
	}

};

#include "../Serializer.h"

