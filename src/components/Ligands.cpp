#include <components/ligands.h>

LigandGroup::LigandGroup(Vec3& position_) :
		buffers(Compute::SimBuffers::Get()),
		centerPositionLigands(position_)
{
	Sow();
	PushSpheresInBuffer();
	CreateSearchTree();
}

void LigandGroup::Sow()
{
	while (ligands.size() < ligandsCount)
	{
		Vec3 tmp(Randf(-sowRadius, sowRadius), 0, Randf(-sowRadius, sowRadius));
		if (tmp.x*tmp.x + tmp.z*tmp.z < sowRadius)
		{
			tmp.x += centerPositionLigands.x;
			tmp.z += centerPositionLigands.z;
			Ligand* lig = new Ligand(tmp);
			ligands.push_back(lig);
		}
	}
}

void LigandGroup::PushSpheresInBuffer()
{
	Quat q = QuatFromAxisAngle(Vec3(0.0, 1.0, 0.0), 0);

	// add particles to system
	for (auto &ligand : ligands)
	{
		const Vec3 p = ligand->coord;
		float invMass = 0.00000000001f;

		buffers.positions.push_back(Vec4(p.x, p.y, p.z, invMass));
		buffers.velocities.push_back(Vec3(0.0f));
		buffers.phases.push_back(0);
		buffers.restPositions.push_back(Vec4(p.x, p.y, p.z, invMass));

		buffers.numParticles++;
		buffers.maxParticles++;
		buffers.activeIndices.push_back(buffers.positions.size() - 1);

		ligand->index = buffers.positions.size() - 1;
	}
}

void LigandGroup::lock_ligand(int index)
{
	ligands[index]->isFree = false;
}

void LigandGroup::open_ligand(int index)
{
	ligands[index]->isFree = true;
}

std::vector<Ligand*> LigandGroup::get_ligands()
{
	return ligands;
}

void LigandGroup::Update()
{
	PushSpheresInBuffer();
}

void LigandGroup::CreateSearchTree()
{
	for (size_t i = 0; i < ligands.size(); ++i)
	{
		point p = point(ligands[i]->coord.x, ligands[i]->coord.y, ligands[i]->coord.z);
		rtree.insert({ p, i });
	}
}

std::vector<value> LigandGroup::FindCloseLigands(point sought, float searchRadius) {
	// search for nearest neighbours
	std::vector<value> returned_values;
	rtree.query(bgi::satisfies([&](value const& v) {return bg::distance(v.first, sought) < searchRadius; }),
		std::back_inserter(returned_values));

	return returned_values;
}
