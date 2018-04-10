#include <components/receptors.h>

// Receptors 
//////////////////////////////////////////////////////////////////////////////////////

Receptors::Receptors(Shell* shell_) :
	buffers(Compute::SimBuffers::Get())
{
	shell = shell_;

	springs = new Springs(&buffers);
	select_receptors();
}

void Receptors::Update(LigandGroup *ligandGroup)
{
	clear_buffer();
	TrySetSprings(ligandGroup);
	push_springs_in_buffer();
}

void Receptors::clear_buffer()
{
	buffers.springIndices.resize(springs->prevBufferIndSize);
	buffers.springLengths.resize(springs->prevBufferSize);
	buffers.springStiffness.resize(springs->prevBufferSize);
}

void Receptors::push_springs_in_buffer()
{
	springs->prevBufferSize = buffers.springLengths.size();
	springs->prevBufferIndSize = buffers.springIndices.size();

	for (int i = 0; i < springs->springLength.size(); i++)
	{
		buffers.springIndices.push_back(springs->springShellIndices[i]);
		buffers.springIndices.push_back(springs->springLigandIndices[i]);
		buffers.springLengths.push_back(springs->springLength[i]);
		buffers.springStiffness.push_back(springs->springStiffness[i]);
	}
}

void Receptors::TryBreakSprings()
{
	std::list<int> deleteIndexSprins;

	for (int i = 0; i < springs->springLength.size(); i++)
	{
		float len = Length(Vec3(buffers.positions.get(springs->springShellIndices[i])) - Vec3(buffers.positions.get(springs->springLigandIndices[i])));
		if (len > breakRadius)
		{
			auto p = Randf(0, 1);
			if (p < breakProb)
			{
				deleteIndexSprins.push_front(i);
			}
		}
	}
}

void Receptors::TrySetSprings(LigandGroup *ligandGroup)
{
	auto ligands = ligandGroup->get_ligands();

	for (int i = 0; i < receptors.size(); i++)
	{
		Receptor* rec = receptors[i];
		if (rec->isFree)
		{
			const Vec4 &rec_pos = buffers.positions[rec->index];
			auto close_ligands = ligandGroup->FindCloseLigands({ rec_pos.x, rec_pos.y, rec_pos.z }, searchRadius);

			for (int j = 0; j < close_ligands.size(); j++)
			{
				Ligand* lig = ligands[close_ligands[j].second];
				if (lig->isFree)
				{
					auto p = Rand(0, 1);
					if (p < connectionProb)
					{
						set_spring(rec, lig, 0.2f);
						lig->lock_ligand();
						rec->lock_receptor();
						break;
					}
				}
			}
		}
	}
}

void Receptors::set_spring(Receptor *rec, Ligand* lig, float dist_koef)
{
	springs->springShellIndices.push_back(rec->index);
	springs->springLigandIndices.push_back(lig->index);
	springs->springLength.push_back(dist_koef * Length(Vec3(buffers.positions.get(rec->index)) - Vec3(buffers.positions.get(lig->index))));
	springs->springStiffness.push_back(stiffness);
}

void Receptors::select_receptors()
{
	size_t beg_ind = shell->GetIndBeginPosition();
	size_t end_ind = shell->GetIndEndPosition();

	receptors.resize(receptorsCount);

	for (size_t i = 0; i < receptorsCount; i++)
		receptors[i] = new Receptor(Rand(beg_ind, end_ind));
};
