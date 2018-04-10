#ifndef SCENE_CELL_H
#define SCENE_CELL_H

#include <fruit/scene.h>
#include <fruit/controller/compute_controller/sim_buffers.h>
#include <fruit/controller/compute_controller/flex_params.h>

#include <cell.h>
#include <environment.h>

class SceneCell : public Scene {

public:

	// constructors and initialize
	//////////////////////////////////////////////////////
	SceneCell() : Scene() {}
	
	SceneCell(const char* name) : Scene(name) {}

	void Initialize() override;
	
	void InitializeFromFile() override;

	void PostInitialize() override;

	// destroy
	//////////////////////////////////////////////
	void Reset() override;

	// update
	/////////////////////////////////////////////////////
	void Sync() override {};

	void Update() override;

	void Draw() override {};

	//////////////////////////////////////////////////////
	template<class Archive>
	void save(Archive &archive) const {
		archive(*cell, sceneLower, sceneUpper);
	}

	template<class Archive>
	void load(Archive &archive) {
		if (cell == nullptr) {
			cell = new Cell();
			cell->InitializeFromFile();
		}

		archive(*cell, sceneLower, sceneUpper);
	}

private:

	std::unique_ptr<Cell> cell;
	std::unique_ptr<Environment> environment;

	Compute::SimBuffers *buffers;
	Compute::FlexParams *flexParams;

};

#endif // SCENE_CELL_H