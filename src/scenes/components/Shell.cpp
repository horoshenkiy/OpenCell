#include "shell.h"

#include "../../../core/platform.h"

void Shell::AddInflatable(const Mesh* mesh, float overPressure, float invMass, int phase) {
	// create a cloth mesh using the global positions / indices
	const int numParticles = int(mesh->m_positions.size());

	// add particles to system
	indBeginPosition = buffers->positions.size();
	for (size_t i = 0; i < mesh->GetNumVertices(); ++i)	{
		const Vec3 p = Vec3(mesh->m_positions[i]);

		buffers->positions.push_back(Vec4(p.x, p.y, p.z, invMass));
		buffers->restPositions.push_back(Vec4(p.x, p.y, p.z, invMass));

		buffers->velocities.push_back(0.0f);
		buffers->phases.push_back(phase);
	}
	indEndPosition = buffers->positions.size();

	// create asset
	NvFlexExtAsset* cloth = NvFlexExtCreateClothFromMesh((float*)&buffers->positions[indBeginPosition],
		numParticles,
		(int*)&mesh->m_indices[0],
		mesh->GetNumFaces(), 0.4f, 0.0f, 0.0f, 0.0f, 0.0f);

	this->asset = cloth;
	this->splitThreshold = 4.0f;

	// set buffers for flex
	for (int i = 0; i < asset->numTriangles; ++i)
	{
		buffers->triangles.push_back(asset->triangleIndices[i * 3 + 0]);
		buffers->triangles.push_back(asset->triangleIndices[i * 3 + 1]);
		buffers->triangles.push_back(asset->triangleIndices[i * 3 + 2]);
	}

	for (int i = 0; i < asset->numSprings * 2; ++i) {
		//std::cout << asset->springIndices[i] << std::endl;
		buffers->springIndices.push_back(asset->springIndices[i] + indBeginPosition);
	}

	for (int i = 0; i < asset->numSprings; ++i)
	{
		buffers->springStiffness.push_back(asset->springCoefficients[i]);
		buffers->springLengths.push_back(asset->springRestLengths[i]);
	}
}

void Shell::Initialize(SimBuffers *buffers) {
	this->buffers = buffers;

	Mesh* mesh = ImportMesh(GetFilePathByPlatform("../../data/sphere_high.ply").c_str());
	Vec3 lower = Vec3(2.0f + 0 * 2.0f, 0.4f + 0 * 1.2f, 1.0f);

	mesh->Normalize();
	mesh->Transform(TranslationMatrix(Point3(lower)));

	AddInflatable(mesh, 1.0f, 0.25f, NvFlexMakePhase(group, eNvFlexPhaseSelfCollide | eNvFlexPhaseSelfCollideFilter));

	buffers->numSolidParticles = buffers->positions.size();
	buffers->numExtraParticles = buffers->positions.size();

	delete mesh;
}

void Shell::Update() {
	
	for (int i = indBeginPosition; i < indEndPosition; i++) {
		buffers->positions[i].x += urd(gen);
		buffers->positions[i].y += urd(gen);
		buffers->positions[i].z += urd(gen);
		
	}
}