#pragma once

#include "kernel.h"
#include "../../controller/compute_controller/FlexParams.h"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<float, 3, bg::cs::cartesian> point;
typedef std::pair<point, unsigned> value;

class Ligand
{
public:

	Ligand(Vec3 coord_)
	{
		coord = coord_;
		isFree = true;
	}

	Vec3 coord;
	int index;
	bool isFree;
};

class LigandGroup
{
public:

	void Sow()
	{
		Vec3 kerPos = kernel->GetPositionCenter();
		Vec3 tmp;
		while (ligands.size() < ligandsCount)
		{
			tmp.x = Randf(-sowRadius, sowRadius);
			tmp.z = Randf(-sowRadius, sowRadius);
			tmp.y = 0;

			if (tmp.x*tmp.x + tmp.z*tmp.z < sowRadius)
			{
				tmp.x += kerPos.x;
				tmp.z += kerPos.z;
				Ligand* lig = new Ligand(tmp);
				ligands.push_back(lig);
			}
		}
	}

	void pushSpheresInBuffer()
	{
		Quat q = QuatFromAxisAngle(Vec3(0.0, 1.0, 0.0), 0);

			// add particles to system
		indBeginPosition = buffers->positions.size();
		for (auto it = ligands.begin(); it!= ligands.end(); it++)
		{
			const Vec3 p = (*it)->coord;
			float invMass = 0.0001f;

			buffers->positions.push_back(Vec4(p.x, p.y, p.z, invMass));
			buffers->velocities.push_back(Vec3(0.0f));
			buffers->phases.push_back(0);
			buffers->restPositions.push_back(Vec4(p.x, p.y, p.z, invMass));

			buffers->numParticles++;
			buffers->maxParticles++;
			buffers->activeIndices.push_back(buffers->positions.size() - 1);

			(*it)->index = buffers->positions.size() - 1;
		}

		indEndPosition = buffers->positions.size();
	}

	LigandGroup(SimBuffers *buffers_, Kernel* kernel_)
	{
		kernel = kernel_;
		buffers = buffers_;

		Sow();
		pushSpheresInBuffer();
	}

	void lock_ligand(int index)
	{
		ligands[index]->isFree = false;
	}

	void open_ligand(int index)
	{
		ligands[index]->isFree = true;
	}

	std::vector<Ligand*> get_ligands()
	{
		return ligands;
	}

	void Update()
	{
		pushSpheresInBuffer();
	}

private:

	int ligandsCount = 100;
	int sowRadius = 10;

	int indBeginPosition;
	int indEndPosition;

	int baseIndex;

	std::vector<Ligand*> ligands;

	Kernel *kernel;
	SimBuffers *buffers;
};