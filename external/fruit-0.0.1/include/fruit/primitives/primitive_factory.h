#ifndef PRIMITIVE_FACTORY_H
#define PRIMITIVE_FACTORY_H

#include <flex/core/maths.h>
#include <flex/core/mesh.h>

#include <fruit/controller/compute_controller/sim_buffers.h>
#include "primitive.h"

#include <vector>
#include <ppl.h>

#include <fruit/utilits/utilits.h>

namespace FruitWork {
namespace Primitives {

class FactoryMesh {

public:
	Mesh* CreateSphere(double radius, int depth);

private:

	class FactorySphere {
	public:	
		void Configure(double rad);
		void CreateShell(int depth, std::vector<Vec3> &outShell, std::vector<uint32_t> &outTri);

	private:

		void GetNorm(double v1[3], double v2[3], double *out) const;
		
		void Split(double *v1, double *v2, double *v3, int depth, 
			std::vector<Vec3> &shell, std::vector<uint32_t> &outTriangles) const;
		
		void SetTria(double *v1, double *v2, double *v3, 
			std::vector<Vec3> &shell, std::vector<uint32_t> &outTriangles) const;
		
		void Scale(double *v) const; 

		const int id[20][3] = { { 1,0,4 },{ 0,4,9 },{ 5, 4, 9 },{ 4, 8, 5 },{ 4, 1, 8 },
								{ 8, 1, 10 },{ 8, 10, 3 },{ 5, 8, 3 },{ 5, 3, 2 },{ 2, 3, 7 },
								{ 7, 3, 10 },{ 7, 10, 6 },{ 7, 6, 11 },{ 11, 6, 0 },{ 0, 6, 1 },
								{ 6, 10, 1 },{ 9, 11, 0 },{ 9, 2, 11 },{ 9, 5, 2 },{ 7, 11, 2 } };

		double V = 0., W = 0.;
		double vertex[12][3];
		Vec3 radiusVectorOfCenter; // center of cell in time of create
		double radius = 0.;
	};
	
	FactorySphere factorySphere;
	
};

class FactoryCloth {

public:

	FactoryCloth() : buffers(Compute::SimBuffers::Get()) {}
	
	Cloth* CreateCloth(const Mesh* mesh, float invMass, int phase) {//, NvFlexExtAsset** asset) {
		// create a cloth mesh using the global positions / indices
		const int numParticles = int(mesh->m_positions.size());

		// add particles to system
		size_t sizeMeshVer = mesh->GetNumVertices();
		
		size_t oldSizePos = buffers.positions.size(), oldSizeRestPos = buffers.restPositions.size();
		size_t oldSizeVel = buffers.velocities.size(), oldSizePhases = buffers.phases.size();

		buffers.positions.resize(oldSizePos + sizeMeshVer);
		buffers.restPositions.resize(oldSizeRestPos + sizeMeshVer);
		buffers.velocities.resize(oldSizeVel + sizeMeshVer);
		buffers.phases.resize(oldSizePhases + sizeMeshVer);
			
		Concurrency::parallel_for(size_t(0), size_t(sizeMeshVer), [&](size_t i) {
			const Vec3 p = Vec3(mesh->m_positions[i]);

			buffers.positions[oldSizePos + i] = { p.x, p.y, p.z, invMass };
			buffers.restPositions[oldSizeRestPos + i] = { p.x, p.y, p.z, invMass };

			buffers.velocities[oldSizeVel + i] = 0.0f;
			buffers.phases[oldSizePhases + i] = phase;
		});

		// create asset
		NvFlexExtAsset* cloth = NvFlexExtCreateClothFromMesh(
			reinterpret_cast<float*>(&buffers.positions[oldSizePos]),
			numParticles,
			(int*)&mesh->m_indices[0],
			mesh->GetNumFaces(), 0.2f, 0.0f, 0.0f, 0.0f, 0.0f);

		// set buffers for flex
		// triangles
		size_t oldSizeTriangles = buffers.triangles.size();
		buffers.triangles.resize(oldSizeTriangles + cloth->numTriangles * 3);

		Concurrency::parallel_for(size_t(0), size_t(cloth->numTriangles * 3), [&](size_t i) {
			buffers.triangles[oldSizeTriangles + i] = cloth->triangleIndices[i];
		});

		// springs
		size_t oldSizeSpringIndeces = buffers.springIndices.size();
		buffers.springIndices.resize(oldSizeTriangles + cloth->numSprings * 2);
			
		Concurrency::parallel_for(size_t(0), size_t(cloth->numSprings * 2), [&](size_t i) {
			buffers.springIndices[oldSizeSpringIndeces + i] = cloth->springIndices[i] + oldSizePos;
		});

		size_t oldSizeSpringStiff = buffers.springStiffness.size(), oldSizeSpringLen = buffers.springLengths.size();
		
		buffers.springStiffness.resize(oldSizeSpringStiff + cloth->numSprings);
		buffers.springLengths.resize(oldSizeSpringLen + cloth->numSprings);

		Concurrency::parallel_for(size_t(0), size_t(cloth->numSprings), [&](size_t i) {
			buffers.springStiffness[oldSizeSpringStiff + i] = cloth->springCoefficients[i];
			buffers.springLengths[oldSizeSpringLen + i] = cloth->springRestLengths[i];
		});

		return new Cloth(cloth, oldSizePos, buffers.positions.size(), oldSizeTriangles, buffers.triangles.size());
	}

private:

	Compute::SimBuffers &buffers;

};

}
}

#endif // PRIMITIVE_FACTORY_H