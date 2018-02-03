#pragma once

#include "protein.h"
#include "kernel.h"
#include "shell.h"

class Cytoskeleton2
{
public:

	float RandomFloat(float a, float b) {
		float random = ((float)rand()) / (float)RAND_MAX;
		float diff = b - a;
		float r = random * diff;
		return a + r;
	}

	void tryToSow()
	{
		Vec3 y_ax(0.0, 1.0, 0.0);

		auto p = (double)(rand()) / RAND_MAX;
		if (p < p_sow)
		{
			Vec3 kerPos = kernel->GetPositionCenter();
			indBeginPositionShell = shell->GetIndBeginPosition();
			indEndPositionShell = shell->GetIndEndPosition();

			float MinDist = FindMinDistToSetWithAngle(kerPos, main_direction, phi / 2, buffers->positions, indBeginPositionShell, indEndPositionShell);
			float largerRadius = MinDist;
			float smallerRadius = kernel->getRadius() + 0.1f;

			float ksi = (rand() / (float)RAND_MAX *phi) - phi / 2;
			float teta = (rand() / (float)RAND_MAX *phi) - phi / 2;
			float rad = RandomFloat(smallerRadius, largerRadius);

			Quat q_loc = QuatFromAxisAngle(y_ax, ksi);
			Vec3 dirToPoint = Rotate(q_loc, main_direction);
			Vec3 randPoint = kerPos + Normalize(dirToPoint) * rad; // begin of new protein

			Quat q_dir = QuatFromAxisAngle(y_ax, teta);
			Vec3 randDir = Rotate(q_dir, main_direction); // direction of new protein
			randDir = Normalize(randDir);

			Protein* protein = new Protein(randPoint, randDir, q_dir);
			tree.push_back(protein);
		}

		//testFunc();
	}

	void tryToGrow()
	{
		for (auto it : tree)
		{
			if (it->end_type == Actin)
			{
				auto p = (double)(rand()) / RAND_MAX;
				if (p < p_grow)
				{
					auto end_of_protein = it->begin + Normalize(it->direction)*it->halfSectionLength*it->length*2;
					auto dist = FindMinDistToSetWithAngle(end_of_protein, it->direction, phi / 2, buffers->positions, indBeginPositionShell, indEndPositionShell);

					if (dist > it->halfSectionLength*2)
						it->length++;
				}
			}
		}
	}

	void tryToAddArp()
	{
		int prev_size = tree.size();

		for (int i = 0; i < prev_size; i++)
		{
			if (tree[i]->end_type == Actin)
			{
				auto p = (double)(rand()) / RAND_MAX;
				if (p < p_ARP)
				{
					tree[i]->end_type = ARP;

					Vec3 forward_dir = tree[i]->direction;

					auto end_of_protein = tree[i]->begin + Normalize(tree[i]->direction)*tree[i]->halfSectionLength*tree[i]->length*2;

					auto new_forward_pos = end_of_protein;
					Protein* forward_protein1 = new Protein(new_forward_pos, forward_dir, tree[i]->rotation);
					forward_protein1->begin_type = ARP;
					tree[i]->forward_protein = forward_protein1;
					tree.push_back(forward_protein1);

					auto p_angle = (double)(rand()) / RAND_MAX;
					float teta;

					p_angle > 0.5f ? teta = branchin_angle / 2 : teta = -branchin_angle/2;

					Quat q = QuatFromAxisAngle(Vec3(0.0, 1.0, 0.0), teta);
					Vec3 new_direction = Rotate(q, forward_dir);
					new_direction = Normalize(new_direction);

					auto new_angle_pos = end_of_protein;
					Protein* angle_protein1 = new Protein(new_angle_pos, new_direction, tree[i]->rotation*q);
					angle_protein1->begin_type = ARP;
					tree[i]->angle_protein = angle_protein1;
					tree.push_back(angle_protein1);
				}
			}
		}
	}

	void tryToBreak()
	{
		for (auto it = tree.begin(); it != tree.end();)
		{
			if ((*it)->begin_type == Actin)
			{
				auto p = (double)(rand()) / RAND_MAX;
				if (p < p_break)
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
						(*it)->begin += Normalize((*it)->direction)*(*it)->halfSectionLength * 2; 
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

	void testFunc()
	{ 
		{
			Vec3 b(0, 0, 0);
			Vec3 x_ax(1.0, 0.0, 0.0);
			Vec3 z_ax(0.0, 0.0, 1.0);
			Vec3 y_ax(0.0, 1.0, 0.0);
			Vec3 dir(1.0, 0.0, 0.0);

			Vec3 defDir(1.0, 0.0, 1.0);

			Vec3 kerPos = kernel->GetPositionCenter();
			kerPos.y = 0;

			Shape shp;
			Vec3 center;
			Quat q,q1,q2;
			float length = 1;


			Protein* protein = new Protein(b, main_direction, QuatFromAxisAngle(Vec3(0.0, 1.0, 0.0), 0));
			tree.push_back(protein);

			q = QuatFromAxisAngle(y_ax, M_PI/4);
			dir = Rotate(q, main_direction); // direction of new protein
			protein = new Protein(b, dir, q);
			tree.push_back(protein);

			q = QuatFromAxisAngle(y_ax, -M_PI / 4);
			dir = Rotate(q, main_direction); // direction of new protein
			protein = new Protein(b, dir, q);
			tree.push_back(protein);
		}
	}

	void clearShapes()
	{
		shapes.clear();
		buffers->shapeGeometry.resize(0);
		buffers->shapePositions.resize(0);
		buffers->shapeRotations.resize(0);
		buffers->shapePrevPositions.resize(0);
		buffers->shapePrevRotations.resize(0);
		buffers->shapeFlags.resize(0);
	}

	void pushShapesInBuffer()
	{
		for (int i = 0; i < shapes.size(); i++)
		{
			buffers->shapeGeometry.push_back(shapes[i].geometry);
			buffers->shapePositions.push_back(shapes[i].position);
			buffers->shapeRotations.push_back(shapes[i].rotation);
			buffers->shapePrevPositions.push_back(shapes[i].prevPosition);
			buffers->shapePrevRotations.push_back(shapes[i].prevRotation);
			buffers->shapeFlags.push_back(shapes[i].flag);
		}
	}

	void pushInShapes()
	{
		for (auto it : tree)
			shapes.emplace_back(it->makeShape());
	}

	Cytoskeleton2(SimBuffers *buffers_, Kernel* kernel_, Shell* shell_)
	{
		buffers = buffers_;
		kernel = kernel_;
		shell = shell_;
	}


	void Update()
	{
		static int time = 0;

		clearShapes();

		tryToSow();

		tryToAddArp();

		tryToGrow();
		tryToBreak();

		pushInShapes();
		pushShapesInBuffer();
	}

private:
	SimBuffers *buffers;
	Kernel *kernel;
	Shell *shell;

	// position of shell buffer
	int indBeginPositionShell;
	int indEndPositionShell;

	std::vector<Protein*> tree;
	std::vector<Shape> shapes;

	float p_sow   = 0.01f;  // variable parameter
	float p_grow  = 0.05f;  // variable parameter
	float p_break = 0.055f;  // variable parameter
	float p_ARP	  = 0.0005f;  // variable parameter

	float phi = M_PI/4;
	float branchin_angle  = 1.256f;

	Vec3 main_direction = Normalize(Vec3(1.0, 0.0, 1.0));
};