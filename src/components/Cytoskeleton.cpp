#include <components/cytoskeleton.h>
#include <fruit/utilits/utilits.h>

void Cytoskeleton::Update() {
	// Update function will be calling on every iteration of program

	//TODO: ugly hardcode - need to create another mechanism of actin starting
	static int j = 0;
	if (j < 400) {
		j++;
		return;
	}

	if (j == 400) 
		j++, std::clog << "Cytoskeleton create" << std::endl;


	clearShapes(); // clearing shapes in buffer

	tryToSow(); // trying create new actin trees
	tryToAddArp(); // trying to branching
	tryToGrow(); // trying to grow actin
	tryToBreak(); // trying to disassemble actin trees

	pushInShapes(); // send received objects to shapes container
	pushShapesInBuffer(); // send received shapes to buffer
}

void Cytoskeleton::tryToSow() {
	FlexParams &flexParams = FlexParams::Get(); // getting all global parameters

	Vec3 y_ax(0.0, 1.0, 0.0); // creating basic vectors
	Vec3 x_ax(1.0, 0.0, 0.0); // creating basic vectors

	auto p = Randf(0, 1);
	if (p < flexParams.p_sow) // probability check
	{
		Quat q = QuatFromAxisAngle(y_ax, flexParams.directionAngle); // creating quaternion from y basic direction and main actin direction angle
		Vec3 streamDirection = Rotate(q, x_ax); // rotating x axis to direction angle
		streamDirection = Normalize(streamDirection); // normalized vector of main actin growth direction

		Vec3 kerPos = kernel->GetPositionCenter(); // getting kernel position
		kerPos.y -= kernel->getRadius(); // getting kernel radius
		indBeginPositionShell = shell->GetIndBeginPosition(); // getting start index of shell in buffer
		indEndPositionShell = shell->GetIndEndPosition(); // getting end index of shell in buffer

		// TODO: FindMinDistToSetWithAngle - create new method against breaking the shell
		float MinDist = FindMinDistToSetWithAngle(kerPos, streamDirection, phi / 2, buffers.positions, indBeginPositionShell, indEndPositionShell);
		float largerRadius = MinDist;  // maximum actin creation radius
		float smallerRadius = kernel->getRadius() + 0.06f; // minimum actin creation radius

		float ksi = Randf(-phi / 2, phi / 2); // random angle from - phi/2 to phi / 2
		float teta = Randf(-phi / 2, phi / 2); // random angle from - phi/2 to phi / 2
		float rad = Randf(smallerRadius, largerRadius); // random radius from smaller to larger

		Quat q_loc = QuatFromAxisAngle(y_ax, ksi); // creating quaternion for position of new protein
		Vec3 dirToPoint = Rotate(q_loc, streamDirection); // rotating main direction on ksi angle by quaternion
		Vec3 randPoint = kerPos + Normalize(dirToPoint) * rad; // position of begining of new protein

		Quat q_dir = QuatFromAxisAngle(y_ax, teta); // creating quaternion for direction of new protein
		Vec3 randDir = Rotate(q_dir, streamDirection); // direction of new protein
		randDir = Normalize(randDir); // normalized direction of new protein

		Protein* protein = new Protein(randPoint, randDir, q_dir, streamDirection); // creating new protein object
		tree.push_back(protein); // pushing new actin tree into actin trees conatiner
	}
}

void Cytoskeleton::tryToGrow() {
	FlexParams &flexParams = FlexParams::Get(); // getting all global parameters

	for (auto it : tree) // iteratin over all trees
	{
		if (it->end_type == Actin) // if end of tree is Actin - this tree can grow further
		{
			auto p = Randf(0, 1);
			if (p < flexParams.p_grow) // probability check
			{
				auto end_of_protein = it->begin + Normalize(it->direction)*flexParams.sectionLength*it->length; // calculating of protein end position
				// TODO: FindMinDistToSetWithAngle - create new method against breaking the shell
				auto dist = FindMinDistToSetWithAngle(end_of_protein, it->direction, phi / 2, buffers.positions, indBeginPositionShell, indEndPositionShell);

				if (dist > flexParams.sectionLength) // distance check
					it->length++; // grow
			}
		}
	}
}

void Cytoskeleton::tryToAddArp() {
	FlexParams &flexParams = FlexParams::Get(); // getting all global parameters

	int prev_size = tree.size(); // previous tree size

	for (int i = 0; i < prev_size; i++) // loop over previous size only (not iteratig over object that created in this loop)
	{
		if (tree[i]->end_type == Actin) // if end of tree is Actin - this tree can add ARP on ending
		{
			auto end_of_protein = tree[i]->begin + Normalize(tree[i]->direction)*flexParams.sectionLength*tree[i]->length; // calculating of protein end position
			// TODO: FindMinDistToSetWithAngle - create new method against breaking the shell
			auto dist = FindMinDistToSetWithAngle(end_of_protein, tree[i]->direction, phi / 2, buffers.positions, indBeginPositionShell, indEndPositionShell);

			if (dist > flexParams.sectionLength) // distance check
			{
				auto p = Randf(0, 1);
				if (p < flexParams.p_ARP) // probability check
				{
					tree[i]->end_type = ARP; // changing end type of tree

					Vec3 forward_dir = tree[i]->direction; // calculating forward direction

					auto end_of_protein = tree[i]->begin + Normalize(tree[i]->direction)*flexParams.sectionLength*tree[i]->length; // calculating of protein end position

					auto new_forward_pos = end_of_protein; // beginning position of new protein - end of previous protein
					Protein* forward_protein1 = new Protein(new_forward_pos, forward_dir, tree[i]->rotation, tree[i]->streamDirection); // creating new Protein object
					forward_protein1->begin_type = ARP; // begin of new protein is ARP
					tree[i]->forward_protein = forward_protein1; // forward ARP protein of previous protein is new Protein
					tree.push_back(forward_protein1); // sending new protein to current tree

					auto p_angle = Randf(0, 1);
					float teta;
					p_angle > 0.5f ? teta = branchin_angle / 2 : teta = -branchin_angle / 2; // random angle: branching angle / 2 or (-branching angle / 2)

					Quat q = QuatFromAxisAngle(Vec3(0.0, 1.0, 0.0), teta); // creating quaternion from y basic direction and branhing angle
					Vec3 new_direction = Rotate(q, forward_dir); // rotate forward direction on teta angle
					new_direction = Normalize(new_direction); // normalized branching direction

					auto new_angle_pos = end_of_protein; // beginning position of new protein - end of previous protein
					Protein* angle_protein1 = new Protein(new_angle_pos, new_direction, tree[i]->rotation*q, tree[i]->streamDirection); // creating new Protein object
					angle_protein1->begin_type = ARP; // begin of new protein is ARP
					tree[i]->angle_protein = angle_protein1; // angle ARP protein of previous protein is new Protein
					tree.push_back(angle_protein1); // sending new protein to current tree
				}
			}
		}
	}
}

void Cytoskeleton::tryToBreak() {
	FlexParams &flexParams = FlexParams::Get();
	//TODO: this method is ugly, needed to redistribute complexity, choose a different logical mechanism
	for (auto it = tree.begin(); it != tree.end();) // iterating over trees
	{
		if ((*it)->begin_type == Actin) // if begin of tree is Actin - this tree disassemble proteins on beginning
		{
			auto p = Randf(0, 1);
			if (p < flexParams.p_break) // probability check
			{
				(*it)->length--; // disassemble
				if (!(*it)->length) // if new length == 0
				{
					if ((*it)->end_type == ARP) // if protein's begin type is ARP
					{
						(*it)->forward_protein->begin_type = Actin; // change ARP to Actin
						(*it)->angle_protein->begin_type = Actin; // change ARP to Actin
					}
					it = tree.erase(it); // removing tree from trees (length = 0)
				}
				else
				{
					(*it)->begin += Normalize((*it)->direction)*flexParams.sectionLength; // recalculation of the start position
					it++; // to the next iteration
				}
			}
			else
				it++; // to the next iteration
		}
		else
			it++; // to the next iteration
	}
}

void Cytoskeleton::clearShapes()
{
	// clearing all buffer's shape of proteins
	shapes.clear();
	buffers.shapeGeometry.resize(prevSize);
	buffers.shapePositions.resize(prevSize);
	buffers.shapeRotations.resize(prevSize);
	buffers.shapePrevPositions.resize(prevSize);
	buffers.shapePrevRotations.resize(prevSize);
	buffers.shapeFlags.resize(prevSize);
}

void Cytoskeleton::pushShapesInBuffer()
{
	checkPrevSize(); // calculating previous size

	// pushing proteins shapes into buffer
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

void Cytoskeleton::pushInShapes()
{
	for (auto it : tree)
		shapes.emplace_back(it->makeShape()); // make shape from protein and push into shapes container
}

Cytoskeleton::Cytoskeleton(Kernel* kernel_, Shell* shell_)
{
	// Constructor of cytoskeleton (we need to know about kernel and shell)
	kernel = kernel_;
	shell = shell_;

	checkPrevSize();
}

void Cytoskeleton::checkPrevSize()
{
	prevSize = buffers.shapeGeometry.size();
}