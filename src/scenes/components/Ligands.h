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
	bool isFree;
};

class LigandGroup
{
public:

	float RandomFloat(float a, float b) {
		float random = ((float)rand()) / (float)RAND_MAX;
		float diff = b - a;
		float r = random * diff;
		return a + r;
	}

	void Sow()
	{
		Vec3 kerPos = kernel->GetPositionCenter();
		Vec3 tmp;
		while (ligands.size() < ligandsCount)
		{
			tmp.x = RandomFloat(-sowRadius, sowRadius);
			tmp.z = RandomFloat(-sowRadius, sowRadius);
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

	void find()
	{
		bgi::rtree< value, bgi::quadratic<16> > rtree;

		// create some values
		for (unsigned i = 0; i < ligands.size(); ++i)
		{
			point p = point(ligands[i]->coord.x, ligands[i]->coord.y, ligands[i]->coord.z);
			rtree.insert(std::make_pair(p, i));
		}

		// search for nearest neighbours
		std::vector<value> returned_values;
		Vec3 kerPos = kernel->GetPositionCenter();
		point sought = point(kerPos.x, kerPos.y, kerPos.z);

		rtree.query(bgi::satisfies([&](value const& v) {return bg::distance(v.first, sought) < searchRadius; }),
			std::back_inserter(returned_values));

		// print returned values
		value to_print_out;
		for (size_t i = 0; i < returned_values.size(); i++) {
			to_print_out = returned_values[i];
			float x = to_print_out.first.get<0>();
			float y = to_print_out.first.get<1>();
			float z = to_print_out.first.get<2>();
			std::cout << "Select point: " << to_print_out.second << std::endl;
			std::cout << "x: " << x << ", y: " << y << ", z: " << z << std::endl;
		}
	}

	void pushSpheresInBuffer()
	{
		Quat q = QuatFromAxisAngle(Vec3(0.0, 1.0, 0.0), 0);

		for (int i = 0; i < ligands.size(); i++)
			AddSphere(buffers, 0.03f, ligands[i]->coord, q);
	}

	LigandGroup(SimBuffers *buffers_, Kernel* kernel_)
	{
		kernel = kernel_;
		buffers = buffers_;
		Sow();
		pushSpheresInBuffer();
	}

private:

	int ligandsCount = 100;
	int sowRadius = 10;
	int searchRadius = 5;

	std::vector<Ligand*> ligands;

	Kernel *kernel;
	SimBuffers *buffers;
};