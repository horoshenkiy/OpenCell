#ifndef LIGANDS_H
#define LIGANDS_H

#include "kernel.h"
#include <fruit/controller/compute_controller/flex_params.h>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

using namespace FruitWork;

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<float, 3, bg::cs::cartesian> point;
typedef std::pair<point, unsigned> value;

class Ligand
{
public:

	Ligand(Vec3 coord_) {
		coord = coord_;
		isFree = true;
	}

	void lock_ligand() {
		isFree = false;
	}

	void open_ligand() {
		isFree = true;
	}

	Vec3 coord;
	int index;
	bool isFree;
};

class LigandGroup
{
public:

	LigandGroup(Vec3& position_);

	void Update();

	void lock_ligand(int index);

	void open_ligand(int index);

	std::vector<Ligand*> get_ligands();

	std::vector<value> FindCloseLigands(point sought, float searchRadius);

private:
	
	void Sow();

	void PushSpheresInBuffer();

	void CreateSearchTree();

	int ligandsCount = 100;
	float sowRadius = 10;

	int baseIndex;

	std::vector<Ligand*> ligands;
	bgi::rtree< value, bgi::quadratic<16>> rtree;

	Vec3 centerPositionLigands;
	Compute::SimBuffers &buffers;
};

#endif // LIGANDS_H