#include "shell.h"

#include "core/platform.h"

#include "../../utilits/PrimitiveFactories.h"

void Shell::AddCloth(const Mesh* mesh, float overPressure, float invMass, int phase) {
	// create a cloth mesh using the global positions / indices
	const int numParticles = int(mesh->m_positions.size());

	// add particles to system
	indBeginPosition = buffers.positions.size();
	for (size_t i = 0; i < mesh->GetNumVertices(); ++i)	{
		const Vec3 p = Vec3(mesh->m_positions[i]);

		buffers.positions.push_back(Vec4(p.x, p.y, p.z, invMass));
		buffers.restPositions.push_back(Vec4(p.x, p.y, p.z, invMass));

		buffers.velocities.push_back(0.0f);
		buffers.phases.push_back(phase);
	}
	indEndPosition = buffers.positions.size();

	// create asset
	NvFlexExtAsset* cloth = NvFlexExtCreateClothFromMesh(
		reinterpret_cast<float*>(&buffers.positions[indBeginPosition]),
		numParticles,
		(int*)&mesh->m_indices[0],
		mesh->GetNumFaces(), 0.4f, 0.0f, 0.0f, 0.0f, 0.0f);

	this->asset = cloth;
	this->splitThreshold = 4.0f;

	// set buffers for flex
	for (size_t i = 0; i < asset->numTriangles; ++i)
	{
		buffers.triangles.push_back(asset->triangleIndices[i * 3 + 0]);
		buffers.triangles.push_back(asset->triangleIndices[i * 3 + 1]);
		buffers.triangles.push_back(asset->triangleIndices[i * 3 + 2]);
	}

	for (size_t i = 0; i < asset->numSprings * 2; ++i) 
		buffers.springIndices.push_back(asset->springIndices[i] + indBeginPosition);

	for (size_t i = 0; i < asset->numSprings; ++i)
	{
		buffers.springStiffness.push_back(asset->springCoefficients[i]);
		buffers.springLengths.push_back(asset->springRestLengths[i]);
	}
}

void Shell::Initialize() {
	Factory::FactoryMesh factoryMesh;
	Mesh *mesh = factoryMesh.CreateSphere(1.2, 5);

	//Mesh* mesh = CreateSphere(30, 30);//
	//Mesh* mesh = ImportMesh("../../data/sphere_high.ply");
	Vec3 lower = Vec3(2.0f + 0 * 2.0f, 0.4f + 0 * 1.2f, 1.0f);

	//mesh->Normalize();
	mesh->Transform(TranslationMatrix(Point3(lower)));

	AddCloth(mesh, 1.0f, 0.25f, NvFlexMakePhase(group, eNvFlexPhaseSelfCollide | eNvFlexPhaseSelfCollideFilter));

	buffers.numSolidParticles = buffers.positions.size();
	buffers.numExtraParticles = buffers.positions.size();

	delete mesh;
}

void Shell::Update() {
	
	for (size_t i = indBeginPosition; i < indEndPosition; i++) {
		buffers.positions[i].x += urd(gen);
		buffers.positions[i].y += urd(gen);
		buffers.positions[i].z += urd(gen);
		
	}
}