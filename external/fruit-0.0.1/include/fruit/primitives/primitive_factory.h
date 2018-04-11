#ifndef PRIMITIVE_FACTORY_H
#define PRIMITIVE_FACTORY_H

#include <flex/core/maths.h>
#include <flex/core/mesh.h>

#include <fruit/controller/compute_controller/sim_buffers.h>
#include "primitive.h"

#include <vector>

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

	FactoryCloth() {}
	
	Cloth* CreateCloth(const Mesh *mesh, int invMass, int phase, float stretchStiffness, float bendStiffness, float tetherStiffness) {

		Compute::SimBuffers &buffers = Compute::SimBuffers::Get();

		// create a cloth mesh using the global positions / indices
		const int numParticles = int(mesh->m_positions.size());

		// add particles to system
		size_t indBeginPosition = buffers.positions.size();
		
		for (size_t i = 0; i < mesh->GetNumVertices(); ++i) {
			const Vec3 p = Vec3(mesh->m_positions[i]);

			buffers.positions.push_back(Vec4(p.x, p.y, p.z, invMass));
			buffers.restPositions.push_back(Vec4(p.x, p.y, p.z, invMass));

			buffers.velocities.push_back(0.0f);
			buffers.phases.push_back(phase);
		}
		
		size_t indEndPosition = buffers.positions.size();

		// create asset
		NvFlexExtAsset* assetCloth = NvFlexExtCreateClothFromMesh(
			reinterpret_cast<float*>(&buffers.positions[indBeginPosition]),
			numParticles,
			(int*)&mesh->m_indices[0],
			mesh->GetNumFaces(), 
			stretchStiffness, bendStiffness, tetherStiffness, 
			0.0f, 0.0f);

		// set buffers for flex
		for (size_t i = 0; i < assetCloth->numTriangles; ++i) {
			buffers.triangles.push_back(assetCloth->triangleIndices[i * 3 + 0]);
			buffers.triangles.push_back(assetCloth->triangleIndices[i * 3 + 1]);
			buffers.triangles.push_back(assetCloth->triangleIndices[i * 3 + 2]);
		}

		for (size_t i = 0; i < assetCloth->numSprings * 2; ++i)
			buffers.springIndices.push_back(assetCloth->springIndices[i] + indBeginPosition);

		for (size_t i = 0; i < assetCloth->numSprings; ++i) {
			buffers.springStiffness.push_back(assetCloth->springCoefficients[i]);
			buffers.springLengths.push_back(assetCloth->springRestLengths[i]);
		}

		return (new Cloth(assetCloth, indBeginPosition, indEndPosition));
	}

private:

	//Compute::SimBuffers &buffers;

};

}
}

#endif // PRIMITIVE_FACTORY_H