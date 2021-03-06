#pragma once

#include "protein.h"
#include "kernel.h"
#include "shell.h"
#include <fruit/controller/compute_controller/flex_params.h>


class Cytoskeleton2 {

public:

	Cytoskeleton2(Kernel* kernel_, Shell* shell_);

	void Update();

private:

	void tryToSow();
	
	void tryToGrow();
	
	void tryToAddArp();
	
	void tryToBreak();
	
	void clearShapes();
	
	void pushShapesInBuffer();
	
	void pushInShapes();
	
	void checkPrevSize();
	
	SimBuffers &buffers = SimBuffers::Get();
	Kernel* kernel;
	Shell* shell;

	// position of shell buffer
	int indBeginPositionShell;
	int indEndPositionShell;

	std::vector<Protein*> tree;
	std::vector<Shape> shapes;

	int prevSize;

	float phi = M_PI/4;
	float branchin_angle  = 1.256f;

	//Vec3 main_direction = Normalize(Vec3(1.0, 0.0, 1.0));
};