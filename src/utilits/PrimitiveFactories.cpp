#include "PrimitiveFactories.h"

#include <map>
#include <set>
#include <list>

namespace Factory {

// Implementation for FactorySphere
//////////////////////////////////////////////////////////////////////////////////

void FactoryMesh::FactorySphere::Configure(double rad) {
	radius = rad;

	radiusVectorOfCenter.x = 0;
	radiusVectorOfCenter.y = 0;
	radiusVectorOfCenter.z = 0;

	V = radius * cos(3. * atan(1.) / 2.5);
	W = radius * sin(3. * atan(1.) / 2.5);

	// configure vertex of icosahedron
	vertex[0][0] = -V, vertex[0][1] = 0., vertex[0][2] = W;
	vertex[1][0] = V, vertex[1][1] = 0., vertex[1][2] = W;
	vertex[2][0] = -V, vertex[2][1] = 0., vertex[2][2] = -W;
	vertex[3][0] = V, vertex[3][1] = 0., vertex[3][2] = -W;
	vertex[4][0] = 0., vertex[4][1] = W, vertex[4][2] = V;
	vertex[5][0] = 0., vertex[5][1] = W, vertex[5][2] = -V;
	vertex[6][0] = 0., vertex[6][1] = -W, vertex[6][2] = V;
	vertex[7][0] = 0., vertex[7][1] = -W, vertex[7][2] = -V;
	vertex[8][0] = W, vertex[8][1] = V, vertex[8][2] = 0.;
	vertex[9][0] = -W, vertex[9][1] = V, vertex[9][2] = 0.;
	vertex[10][0] = W, vertex[10][1] = -V, vertex[10][2] = 0.;
	vertex[11][0] = -W, vertex[11][1] = -V, vertex[11][2] = 0.;
}

void FactoryMesh::FactorySphere::Scale(double *v) const {
	
	double d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

	if (d == 0.) {
		//MessageBox(0, L"Zero length vector", L"Error", MB_OK);
	}
	else {
		v[0] /= d;
		v[1] /= d;
		v[2] /= d;
		v[0] *= radius;
		v[1] *= radius;
		v[2] *= radius;
	}
}

void FactoryMesh::FactorySphere::GetNorm(double v1[3], double v2[3], double *out) const {
	out[0] = v1[1] * v2[2] - v1[2] * v2[1];
	out[1] = v1[2] * v2[0] - v1[0] * v2[2];
	out[2] = v1[0] * v2[1] - v1[1] * v2[0];
	Scale(out);
}

void FactoryMesh::FactorySphere::Split(double *v1, double *v2, double *v3, int depth, 
									   std::vector<Vec3> &shell, std::vector<uint32_t> &outTriangles) const {
	double v12[3], v23[3], v31[3];

	if (depth == 0) {
		SetTria(v1, v2, v3, shell, outTriangles);

		return;
	}

	for (int i = 0; i< 3; i++) {
		v12[i] = v1[i] + v2[i];
		v23[i] = v2[i] + v3[i];
		v31[i] = v3[i] + v1[i];

	}

	Scale(v12);
	Scale(v23);
	Scale(v31);
	Split(v1, v12, v31, depth - 1, shell, outTriangles);
	Split(v2, v23, v12, depth - 1, shell, outTriangles);
	Split(v3, v31, v23, depth - 1, shell, outTriangles);
	Split(v12, v23, v31, depth - 1, shell, outTriangles);

}

void FactoryMesh::FactorySphere::SetTria(double *v1, 
										double *v2, 
										double *v3, 
										std::vector<Vec3> &shell, 
										std::vector<uint32_t> &outTriangles) const 
{
	int counterVertex1 = 0, index1 = -1;
	int counterVertex2 = 0, index2 = -1;
	int counterVertex3 = 0, index3 = -1;

	Vec3 vertex1, vertex2, vertex3;
	vertex1.x = v1[0] + radiusVectorOfCenter.x;
	vertex1.y = v1[1] + radiusVectorOfCenter.y;
	vertex1.z = v1[2] + radiusVectorOfCenter.z;

	vertex2.x = v2[0] + radiusVectorOfCenter.x;
	vertex2.y = v2[1] + radiusVectorOfCenter.y;
	vertex2.z = v2[2] + radiusVectorOfCenter.z;

	vertex3.x = v3[0] + radiusVectorOfCenter.x;
	vertex3.y = v3[1] + radiusVectorOfCenter.y;
	vertex3.z = v3[2] + radiusVectorOfCenter.z;

	if (shell.size() != 0) {
		for (auto i = 0; i < shell.size(); i++) {
			if (shell.at(i).x == vertex1.x && shell.at(i).y == vertex1.y && shell.at(i).z == vertex1.z) {
				index1 = i;
				counterVertex1++;
			}

			if (shell.at(i).x == vertex2.x && shell.at(i).y == vertex2.y && shell.at(i).z == vertex2.z) {
				index2 = i;
				counterVertex2++;
			}

			if (shell.at(i).x == vertex3.x && shell.at(i).y == vertex3.y && shell.at(i).z == vertex3.z) {
				index3 = i;
				counterVertex3++;
			}
		}
	}

	if (!counterVertex1) {
		index1 = shell.size();
		shell.push_back(vertex1);
	}

	if (!counterVertex2) {
		index2 = shell.size();
		shell.push_back(vertex2);
	}

	if (!counterVertex3) {
		index3 = shell.size();
		shell.push_back(vertex3);
	}

	// for correct calculate normals
	outTriangles.push_back(index1);
	outTriangles.push_back(index2);
	outTriangles.push_back(index3);
}

void FactoryMesh::FactorySphere::CreateShell(int depth, std::vector<Vec3> &outShell, std::vector<uint32_t> &outTriangles)
{
	for (int i = 0; i < 20; i++) 
		Split(vertex[id[i][0]], vertex[id[i][1]], vertex[id[i][2]], depth, outShell, outTriangles);
}

// Implementation for Factory Mesh

Mesh* FactoryMesh::CreateSphere(double radius, int depth) {
	Mesh *mesh = new Mesh();
	
	std::vector<Vec3> vectorPoints;
	std::vector<uint32_t> triangles;

	try {
		factorySphere.Configure(radius);
		factorySphere.CreateShell(depth, vectorPoints, triangles);
	} catch (std::exception &ex) {
		throw std::exception("FactoryMesh: Fatal error with create triangulate Sphere");
	}


	for (int i = 0; i < vectorPoints.size(); i++) {
		mesh->m_positions.push_back(Point3(vectorPoints[i]));
		mesh->m_normals.push_back(vectorPoints[i] / radius);
	}

	for (auto it : triangles)
		mesh->m_indices.push_back(it);

	return mesh;
}


}