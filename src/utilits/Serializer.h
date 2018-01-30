#pragma once

#include <maths.h>
#include <NvFlexImplFruitExt.h>

#include "../scenes/components/kernel.h"
#include "../scenes/components/cytoplasm.h"
#include "../scenes/components/shell.h"

#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>

#include "../include/serialize_types.h"

#ifdef _WINDOWS
#include <windows.h>
#endif

class SceneCell;
class Cell;
class SerializerTest;

class Serializer {
public:

	///// constructors
	///////////////////////////////////////////////////////////
	Serializer() = default;

	Serializer(SceneCell *sceneCell) :
		buffers(&SimBuffers::Get()),
		renderBuffers(&RenderBuffers::Get()),
		sceneCell(sceneCell) {}

	////// getters and setters
	////////////////////////////////////////////////////////////
	void SetIsNeedSave(bool isNeedSave) {
		this->isNeedSave = isNeedSave;
	}
	
	bool GetIsNeedSave() {
		return isNeedSave;
	}

	///// save and load functions 
	//////////////////////////////////////////////////////////////
	bool SaveStateBinary(std::string nameState) {
		if (!DirExists("../../data/states"))
			CreateDirectory("../../data/states", nullptr);

		std::string pathState = "../../data/states/" + nameState;
		if (DirExists(pathState))
			return false;

		CreateDirectory(pathState.c_str(), nullptr);

		if (sceneCell == nullptr || buffers == nullptr) {
			printf("Seriliazer: scene_cell or buffers are n't initialize!");
			return false;
		}

		std::ofstream out("../../data/states/" + nameState + "/StateBinary.bin", std::ofstream::binary);

		cereal::BinaryOutputArchive archive(out);
		Save(archive, *(this->sceneCell));
		Save(archive, *(this->buffers));
		Save(archive, *(this->renderBuffers));

		out.close();

		return true;
	}

	bool LoadStateBinary(std::string nameState) {
		if (!DirExists("../../data/states/" + nameState))
			return false;

		std::ifstream in("../../data/states/" + nameState + "/StateBinary.bin", std::ofstream::binary);

		cereal::BinaryInputArchive archive(in);
		Load(archive, *(this->sceneCell));
		Load(archive, *(this->buffers));
		Load(archive, *(this->renderBuffers));
		
		in.close();

		return true;
	}

private:

	friend SerializerTest;

	SceneCell *sceneCell;
	SimBuffers *buffers;
	RenderBuffers *renderBuffers;

	bool isNeedSave = false;
	
	///// For primitive
	///////////////////////////////////////////////////////
	template<class Archive, typename T>
	void Save(Archive &archive, T &a) {
		archive(a);
	}

	template<class Archive, typename T>
	void Load(Archive &archive, T &a) {
		archive(a);
	}

	///// help functions 
	///////////////////////////////////////////////////////
	
	#ifdef _WINDOWS
	
	bool DirExists(const std::string &dirName) {
		DWORD fileType = GetFileAttributesA(dirName.c_str());

		if (fileType == INVALID_FILE_ATTRIBUTES)
			return false;

		if (fileType == FILE_ATTRIBUTE_DIRECTORY)
			return true;

		return false;
	}

	#endif

};

#include "../scenes/SceneCell.h"
#include "../scenes/cell.h"