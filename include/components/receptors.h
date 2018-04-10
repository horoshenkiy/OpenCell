#ifndef RECEPTORS_H
#define RECEPTORS_H

#include <vector>
#include <set>
#include <random>

#include "shell.h"
#include "component.h"
#include "ligands.h"

struct Receptor
{
	Receptor(int ind) {
		index = ind;
		isFree = true;
	}

	void lock_receptor() {
		isFree = false;
	}

	void open_receptor() {
		isFree = true;
	}

	int index;
	bool isFree;
};

struct Springs
{
	std::vector<int> springShellIndices;
	std::vector<int> springLigandIndices;
	std::vector<float> springLength;
	std::vector<float> springStiffness;

	size_t prevBufferSize;
	size_t prevBufferIndSize;

	Springs(Compute::SimBuffers* buffers_)
	{
		prevBufferSize = buffers_->springLengths.size();
		prevBufferIndSize = buffers_->springIndices.size();
	}
};

class Receptors {

public:

	// constructors
	Receptors(Shell* shell_);

	// updates
	void Update(LigandGroup* ligandGroup);

	void clear_buffer();

private:

	// methods
	////////////////////////////////////////////////////////////

	void select_receptors();

	void TrySetSprings(LigandGroup *ligandGroup);

	void set_spring(Receptor *rec, Ligand* lig, float dist_koef);

	void TryBreakSprings();

	void push_springs_in_buffer();

	// values
	/////////////////////////////////////////////////////////////

	std::vector<Receptor*> receptors;

	Springs* springs;

	int receptorsCount = 100;

	float connectionProb = 1;
	float breakProb = 1;

	float searchRadius = 1.0;
	float breakRadius = 0.5;
	float stiffness = 0.5;

	Compute::SimBuffers &buffers;
	Shell* shell;
};

#endif // RECEPTORS_H