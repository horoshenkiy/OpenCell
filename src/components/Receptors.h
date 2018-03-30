#pragma once

#include <vector>
#include <set>
#include <random>

#include "shell.h"
#include "component.h"
#include "Ligands.h"


struct Receptor
{
	Receptor(int ind)
	{
		index = ind;
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

	//	buffers->springIndices.push_back(i);
	//buffers->springIndices.push_back(j);
	//buffers->springLengths.push_back((1.0f + give)*Length(Vec3(buffers->positions.get(i)) - Vec3(buffers->positions.get(j))));
	//buffers->springStiffness.push_back(stiffness);

	int prevBufferSize;
	int prevBufferIndSize;

	int springsCount;
};

class Receptors {

public:

	Receptors(SimBuffers* buffers_, Shell* shell_, LigandGroup* ligands_group_)
	{
		buffers = buffers_;
		shell = shell_;
		ligands_group = ligands_group_;

		select_receptors();
		create_search_tree();
	}

	void select_receptors()
	{
		int beg_ind = shell->GetIndBeginPosition();
		int end_ind = shell->GetIndEndPosition();

		receptors.resize(receptorsCount);

		for (int i = 0; i < receptorsCount; i++)
			receptors[i] = new Receptor(Rand(beg_ind, end_ind));
	};

	//void set_spring(int indexOfShellPoint, int indexOfLigand, float stiffness)
	//{
	//	// i - shell point; i+1 - indexOfLigand
	//	springs->springShellIndices.push_back(indexOfShellPoint);
	//	springs->springLigandIndices.push_back(indexOfLigand);
	//	springs->springLength.push_back(Length(Vec3(buffers->positions.get(indexOfShellPoint)) - Vec3(buffers->positions.get(indexOfLigand))));
	//	springs->springStiffness.push_back(stiffness);

	//	springs->springsCount++;
	//}

	void set_spring(Receptor *rec, Ligand* lig)
	{
		springs->springShellIndices.push_back(rec->index);
		springs->springLigandIndices.push_back(lig->index);
		springs->springLength.push_back(Length(Vec3(buffers->positions.get(rec->index)) - Vec3(buffers->positions.get(lig->index))));
		springs->springStiffness.push_back(stiffness);
	}
	
	void create_search_tree()
	{
		auto ligands = ligands_group->get_ligands();

		for (unsigned i = 0; i < ligands.size(); ++i)
		{
			point p = point(ligands[i]->coord.x, ligands[i]->coord.y, ligands[i]->coord.z);
			rtree.insert(std::make_pair(p, i));
		}
	}

	std::vector<value> find_close_ligands(Receptor* rec)
	{
		// search for nearest neighbours
		std::vector<value> returned_values;

		auto rec_pos = buffers->positions[rec->index];
		point sought = point(rec_pos.x, rec_pos.y, rec_pos.z);

		rtree.query(bgi::satisfies([&](value const& v) {return bg::distance(v.first, sought) < searchRadius; }),
			std::back_inserter(returned_values));

		//// print returned values
		//value to_print_out;
		//for (size_t i = 0; i < returned_values.size(); i++) {
			//to_print_out = returned_values[i];
			//float x = to_print_out.first.get<0>();
			//float y = to_print_out.first.get<1>();
			//float z = to_print_out.first.get<2>();
			//std::cout << "Select point: " << to_print_out.second << std::endl;
			//std::cout << "x: " << x << ", y: " << y << ", z: " << z << std::endl;
		//}

		return returned_values;
	}


	void TrySetSprings()
	{
		auto ligands = ligands_group->get_ligands();

		for (int i = 0; i < receptors.size(); i++)
		{
			Receptor* rec = receptors[i];
			if (rec->isFree)
			{
				auto close_ligands = find_close_ligands(receptors[i]);

				for (int j = 0; j < close_ligands.size(); j++)
				{
					Ligand* lig = ligands[close_ligands[j].second];
					if (lig->isFree)
					{
						auto p = Rand(0, 1);
						if (p < connectionProb)
						{
							set_spring(rec, lig);
							break;
						}
					}		
				}
			}		
		}
	}

	void TryBreakSprings()
	{
		std::list<int> deleteIndexSprins;

		for (int i = 0; i < springs->springsCount; i++)
		{
			float len = Length(Vec3(buffers->positions.get(springs->springShellIndices[i])) - Vec3(buffers->positions.get(springs->springLigandIndices[i])));
			if (len > breakRadius)
			{
				auto p = Randf(0, 1);
				if (p < breakProb)
				{
					deleteIndexSprins.push_front(i);
				}
			}
		}

		//for (int i = 0; i < deleteIndexSprins.size(); i++)
		//	delete_spring(i);
	}

	//void delete_spring(int index)
	//{
	//	springs->springShellIndices.erase(springs->springShellIndices.begin() + index);
	//	springs->springLigandIndices.erase(springs->springLigandIndices.begin() + index);
	//	springs->springLength.erase(springs->springLength.begin() + index);
	//	springs->springStiffness.erase(springs->springStiffness.begin() + index);

	//	springs->springsCount--;
	//}

	void push_springs_in_buffer()
	{
		springs->prevBufferSize = buffers->springLengths.size();
		springs->prevBufferIndSize = buffers->springIndices.size();

		for (int i = 0; i < springs->springsCount; i++)
		{
			buffers->springIndices.push_back(springs->springShellIndices[i]);
			buffers->springIndices.push_back(springs->springLigandIndices[i]);
			buffers->springLengths.push_back(springs->springLength[i]);
			buffers->springStiffness.push_back(springs->springStiffness[i]);
		}
	}

	void clear_buffer()
	{
		buffers->springIndices.resize(springs->prevBufferIndSize);
		buffers->springLengths.resize(springs->prevBufferSize);
		buffers->springStiffness.resize(springs->prevBufferSize);
	}

	void Update()
	{
		clear_buffer();
		TrySetSprings();
		push_springs_in_buffer();
	}

private:

	std::vector<Receptor*> receptors;
	Springs* springs;
	LigandGroup* ligands_group;

	int receptorsCount = 100;

	float connectionProb = 1.0f;
	float breakProb = 1.0f;

	bgi::rtree< value, bgi::quadratic<16>> rtree;
	float searchRadius = 1.0f;
	float breakRadius = 1.2f;
	float stiffness = 1.0f;

	SimBuffers* buffers;
	Shell* shell;
};