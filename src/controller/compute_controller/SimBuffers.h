#pragma once

#include "../../../fruit_extensions/NvFlexImplFruitExt.h"
#include "../../Utilits.h"

#include "FruitSimBuffers.h"

struct Shape {
	NvFlexCollisionGeometry geometry;
	
	Vec4 position;
	Quat rotation;

	Vec4 prevPosition;
	Quat prevRotation;

	int flag;
};

struct SimBuffers : public FruitSimBuffers<FruitNvFlexVector>
{
	//construtors, destructors and in initialize
	////////////////////////////////////////
	SimBuffers(NvFlexLibrary* l);
	~SimBuffers();

	void Initialize();
	void PostInitialize();

	//mapping and unmapping buffers with inner buffers in Flex
	//////////////////////////////////////////////////////////
	void MapBuffers();
	void UnmapBuffers();

	// build constraints and send data		
	///////////////////////////////////////////////////////////////
	void BuildConstraints();
	void SendBuffers(NvFlexSolver *flex);

	// methods for clearing
	///////////////////////////////////////////////////////////////
	void ClearShapes();
};