#include "cytoskeleton2.h"
#include "fruit/utilits/utilits.h"

void Cytoskeleton2::Update() {	
	clearShapes();

	tryToSow();
	tryToAddArp();
	tryToGrow();
	tryToBreak();

	pushInShapes();
	pushShapesInBuffer();
}

void Cytoskeleton2::tryToSow() {
	FlexParams &flexParams = FlexParams::Get();

	Vec3 y_ax(0.0, 1.0, 0.0);
	Vec3 x_ax(1.0, 0.0, 0.0);

	auto p = Randf(0, 1);
	if (p < flexParams.p_sow)
	{

		Quat q = QuatFromAxisAngle(y_ax, flexParams.directionAngle); // direction of stream from directionAngle
		Vec3 streamDirection = Rotate(q, x_ax);
		streamDirection = Normalize(streamDirection);

		Vec3 kerPos = kernel->GetPositionCenter();
		kerPos.y -= kernel->getRadius();
		indBeginPositionShell = shell->GetIndBeginPosition();
		indEndPositionShell = shell->GetIndEndPosition();

		float MinDist = FindMinDistToSetWithAngle(kerPos, streamDirection, phi / 2, buffers.positions, indBeginPositionShell, indEndPositionShell);
		float largerRadius = MinDist;
		float smallerRadius = kernel->getRadius() + 0.06f;

		float ksi = Randf(-phi / 2, phi / 2);
		float teta = Randf(-phi / 2, phi / 2);
		float rad = Randf(smallerRadius, largerRadius);

		Quat q_loc = QuatFromAxisAngle(y_ax, ksi);
		Vec3 dirToPoint = Rotate(q_loc, streamDirection);
		Vec3 randPoint = kerPos + Normalize(dirToPoint) * rad; // begin of new protein

		Quat q_dir = QuatFromAxisAngle(y_ax, teta);
		Vec3 randDir = Rotate(q_dir, streamDirection); // direction of new protein
		randDir = Normalize(randDir);

		Protein* protein = new Protein(randPoint, randDir, q_dir, streamDirection);
		tree.push_back(protein);
	}
}

void Cytoskeleton2::tryToGrow() {
	FlexParams &flexParams = FlexParams::Get();

	for (auto it : tree)
	{
		if (it->end_type == Actin)
		{
			auto p = Randf(0, 1);
			if (p < flexParams.p_grow)
			{
				auto end_of_protein = it->begin + Normalize(it->direction)*flexParams.sectionLength*it->length;
				auto dist = FindMinDistToSetWithAngle(end_of_protein, it->direction, phi / 2, buffers.positions, indBeginPositionShell, indEndPositionShell);

				if (dist > flexParams.sectionLength)
					it->length++;
			}
		}
	}
}

void Cytoskeleton2::tryToAddArp() {
	FlexParams &flexParams = FlexParams::Get();

	int prev_size = tree.size();

	for (int i = 0; i < prev_size; i++)
	{
		if (tree[i]->end_type == Actin)
		{
			auto end_of_protein = tree[i]->begin + Normalize(tree[i]->direction)*flexParams.sectionLength*tree[i]->length;
			auto dist = FindMinDistToSetWithAngle(end_of_protein, tree[i]->direction, phi / 2, buffers.positions, indBeginPositionShell, indEndPositionShell);

			if (dist > flexParams.sectionLength)
			{
				auto p = Randf(0, 1);
				if (p < flexParams.p_ARP)
				{
					tree[i]->end_type = ARP;

					Vec3 forward_dir = tree[i]->direction;

					auto end_of_protein = tree[i]->begin + Normalize(tree[i]->direction)*flexParams.sectionLength*tree[i]->length;

					auto new_forward_pos = end_of_protein;
					Protein* forward_protein1 = new Protein(new_forward_pos, forward_dir, tree[i]->rotation, tree[i]->streamDirection);
					forward_protein1->begin_type = ARP;
					tree[i]->forward_protein = forward_protein1;
					tree.push_back(forward_protein1);

					auto p_angle = Randf(0, 1);
					float teta;

					p_angle > 0.5f ? teta = branchin_angle / 2 : teta = -branchin_angle / 2;

					Quat q = QuatFromAxisAngle(Vec3(0.0, 1.0, 0.0), teta);
					Vec3 new_direction = Rotate(q, forward_dir);
					new_direction = Normalize(new_direction);

					auto new_angle_pos = end_of_protein;
					Protein* angle_protein1 = new Protein(new_angle_pos, new_direction, tree[i]->rotation*q, tree[i]->streamDirection);
					angle_protein1->begin_type = ARP;
					tree[i]->angle_protein = angle_protein1;
					tree.push_back(angle_protein1);
				}
			}
		}
	}
}

void Cytoskeleton2::tryToBreak() {
	FlexParams &flexParams = FlexParams::Get();

	for (auto it = tree.begin(); it != tree.end();)
	{
		if ((*it)->begin_type == Actin)
		{
			auto p = Randf(0, 1);
			if (p < flexParams.p_break)
			{
				(*it)->length--;
				if (!(*it)->length)
				{
					if ((*it)->end_type == ARP)
					{
						(*it)->forward_protein->begin_type = Actin;
						(*it)->angle_protein->begin_type = Actin;
					}
					it = tree.erase(it);
				}
				else
				{
					(*it)->begin += Normalize((*it)->direction)*flexParams.sectionLength;
					it++;
				}
			}
			else
				it++;
		}
		else
			it++;
	}
}

void Cytoskeleton2::clearShapes()
{
	shapes.clear();
	buffers.shapeGeometry.resize(prevSize);
	buffers.shapePositions.resize(prevSize);
	buffers.shapeRotations.resize(prevSize);
	buffers.shapePrevPositions.resize(prevSize);
	buffers.shapePrevRotations.resize(prevSize);
	buffers.shapeFlags.resize(prevSize);
}

void Cytoskeleton2::pushShapesInBuffer()
{
	checkPrevSize();

	for (int i = 0; i < shapes.size(); i++)
	{
		buffers.shapeGeometry.push_back(shapes[i].geometry);
		buffers.shapePositions.push_back(shapes[i].position);
		buffers.shapeRotations.push_back(shapes[i].rotation);
		buffers.shapePrevPositions.push_back(shapes[i].prevPosition);
		buffers.shapePrevRotations.push_back(shapes[i].prevRotation);
		buffers.shapeFlags.push_back(shapes[i].flag);
	}
}

void Cytoskeleton2::pushInShapes()
{
	for (auto it : tree)
		shapes.emplace_back(it->makeShape());
}

Cytoskeleton2::Cytoskeleton2(Kernel* kernel_, Shell* shell_)
{
	kernel = kernel_;
	shell = shell_;

	checkPrevSize();
}

void Cytoskeleton2::checkPrevSize()
{
	prevSize = buffers.shapeGeometry.size();
}