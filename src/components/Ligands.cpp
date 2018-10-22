#include <components/ligands.h>

LigandGroup::LigandGroup(Vec3& position_) :
		buffers(Compute::SimBuffers::Get()),
		centerPositionLigands(position_)
{
	// this function will be called when LigandGroup will begin to construct
	Sow(); // creating ligands
	PushSpheresInBuffer(); // send ligands to buffer
	CreateSearchTree(); // send ligands to search tree
}

void LigandGroup::Sow()
{
	// creating ligands
	while (ligands.size() < ligandsCount) // while current ligands count < needed ligands count
	{
		Vec3 tmp(Randf(-sowRadius, sowRadius), 0, Randf(-sowRadius, sowRadius)); // vector with random x from -max radius to max radius and random z -max radius to max radius
		if (tmp.x*tmp.x + tmp.z*tmp.z < sowRadius) // if randius < max radius
		{
			tmp.x += centerPositionLigands.x; // place tmp vector into circle with radius = sowRadius and center = centerPositionLigands
			tmp.z += centerPositionLigands.z;
			Ligand* lig = new Ligand(tmp); // creating new Ligand
			ligands.push_back(lig); // pushin ligang to LigandGroup
		}
	}
}

void LigandGroup::PushSpheresInBuffer()
{
	// add particles to buffer
	for (auto &ligand : ligands) // iteratin over ligands
	{
		const Vec3 p = ligand->coord;
		float invMass = 0.00000000001f; // 1/m , where m - is very very large value (we think ligands are still)

		buffers.positions.push_back(Vec4(p.x, p.y, p.z, invMass));
		buffers.velocities.push_back(Vec3(0.0f)); // initial velocity
		buffers.phases.push_back(0);
		buffers.restPositions.push_back(Vec4(p.x, p.y, p.z, invMass)); // rest position is current position

		buffers.numParticles++; // increase number of particles
		buffers.maxParticles++; // increase max number of particles
		buffers.activeIndices.push_back(buffers.positions.size() - 1); // change active indices of buffer

		ligand->index = buffers.positions.size() - 1; // index of current ligand in buffer
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
	// this function will be calling every iteration
	PushSpheresInBuffer();
}

void LigandGroup::CreateSearchTree()
{
	// creating data structure for fast searching ligands
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
