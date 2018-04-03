#ifndef PRIMITIVE_FACTORY_H
#define PRIMITIVE_FACTORY_H

#include <flex/core/maths.h>
#include <flex/core/mesh.h>

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

}
}

#endif // PRIMITIVE_FACTORY_H