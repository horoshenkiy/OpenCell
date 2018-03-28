#pragma once 

#include <flex/core/cloth.h>
#include <flex/core/mesh.h>

#include <flex/NvFlex.h>
#include <Fruit.h>
#include <stdarg.h>

struct SimBuffers;
struct RenderBuffers;

struct Shape;
class SceneCell;

///////////////////////////////////////////////////////////
// simple helper functions
///////////////////////////////////////////////////////////

inline float sqr(float x) { return x*x; }

float SampleSDF(const float* sdf, int dim, int x, int y, int z);

void CreateSDF(const Mesh* mesh, uint32_t dim, Vec3 lower, Vec3 upper, float* sdf);

NvFlexDistanceFieldId CreateSDF(RenderBuffers *renderBuffers, const char* meshFile, int dim, float margin = 0.1f, float expand = 0.0f);

Vec3 SampleSDFGrad(const float* sdf, int dim, int x, int y, int z);

void GetParticleBounds(SimBuffers *buffers, Vec3& lower, Vec3& upper);

//////////////////////////////////////////////////////////////////////////////////////////
// functions for calculate
////////////////////////////////////////////////////////////////////////////////////////

// calculates local space positions given a set of particles and rigid indices
void CalculateRigidLocalPositions(const Vec4* restPositions, size_t numRestPositions, const int* offsets, const int* indices, int numRigids, Vec3* localPositions);

// for position with mass
float FindMinDistToSet(Vec3 point, FruitVector<Vec4> &bufferOfSet, int startSet, int endSet);
float FindMinDistToSetWithAngle(Vec3 point, Vec3 direction, float angle, FruitVector<Vec4> &bufferOfSet, int startSet, int endSet);
float angleBtwVectors(const Vec3& a, const Vec3& b);
Vec3 CalculateMean(const Vec3* particles, const int* indices, int numIndices);

float CalculateRadius(const Vec3* particles, Vec3 center, const int* indices, int numIndices);

// calculates the union bounds of all the collision shapes in the scene
void GetShapeBounds(SimBuffers *buffers, Vec3& totalLower, Vec3& totalUpper);

////////////////////////////////////////////////////////////////
// it's structs to need for creating functions
////////////////////////////////////////////////////////////////

struct Tri
{
	int a;
	int b;
	int c;

	Tri(int a, int b, int c) : a(a), b(b), c(c) {}

	bool operator < (const Tri& rhs)
	{
		if (a != rhs.a)
			return a < rhs.a;
		else if (b != rhs.b)
			return b < rhs.b;
		else
			return c < rhs.c;
	}
};

struct TriKey
{
	int orig[3];
	int indices[3];

	TriKey(int a, int b, int c)
	{
		orig[0] = a;
		orig[1] = b;
		orig[2] = c;

		indices[0] = a;
		indices[1] = b;
		indices[2] = c;

		std::sort(indices, indices + 3);
	}

	bool operator < (const TriKey& rhs) const
	{
		if (indices[0] != rhs.indices[0])
			return indices[0] < rhs.indices[0];
		else if (indices[1] != rhs.indices[1])
			return indices[1] < rhs.indices[1];
		else
			return indices[2] < rhs.indices[2];
	}
};

struct Cluster
{
	Vec3 mean;
	float radius;

	// indices of particles belonging to this cluster
	std::vector<int> indices;
};

struct Seed
{
	int index;
	float priority;

	bool operator < (const Seed& rhs) const
	{
		return priority < rhs.priority;
	}
};

//////////////////////////////////////////////////////////////
// functions for create
////////////////////////////////////////////////////////

// TODO: Add namespace

// create rigid capsule

void CreateRigidCapsule(SimBuffers &buffers, RenderBuffers &renderBuffers,
						float radius, float halfLength, int slices, int segments,
						Vec3 lower, Vec3 scale, float rotation, float samplingMesh,
						Vec3 velocity, float mass, int phase);

void CreateParticleGrid(SimBuffers *buffers,
						Vec3 lower,
						int dimx, int dimy, int dimz,
						float radius,
						Vec3 velocity,
						float invMass,
						bool rigid, float rigidStiffness,
						int phase, float jitter = 0.005f);

void CreateParticleSphere(SimBuffers *buffers,
						Vec3 center,
						int dim, float radius,
						Vec3 velocity,
						float invMass,
						bool rigid, float rigidStiffness,
						int phase,
						float jitter = 0.005f);

void CreateSpring(SimBuffers *buffers, int i, int j, float stiffness, float give = 0.0f);

// wrapper to create shape from a filename
void CreateParticleShape(SimBuffers *buffers,
						RenderBuffers *renderBuffers,
						const char* filename,
						Vec3 lower, Vec3 scale,
						float rotation, float spacing,
						Vec3 velocity,
						float invMass,
						bool rigid, float rigidStiffness,
						int phase,
						bool skin,
						float jitter = 0.005f,
						Vec3 skinOffset = 0.0f,
						float skinExpand = 0.0f,
						Vec4 color = Vec4(0.0f),
						float springStiffness = 0.0f);

void CreateParticleShape(SimBuffers *buffers,
						RenderBuffers *renderBuffers,
						const Mesh* srcMesh,
						Vec3 lower,
						Vec3 scale,
						float rotation,
						float spacing,
						Vec3 velocity,
						float invMass,
						bool rigid,
						float rigidStiffness,
						int phase,
						bool skin,
						float jitter = 0.005f,
						Vec3 skinOffset = 0.0f,
						float skinExpand = 0.0f,
						Vec4 color = Vec4(0.0f),
						float springStiffness = 0.0f);

void CreateRandomBody(SimBuffers *buffers,
					int numPlanes,
					Vec3 position,
					float minDist,
					float maxDist,
					Vec3 axis,
					float angle,
					float invMass,
					int phase,
					float stiffness);
						

NvFlexTriangleMeshId CreateTriangleMesh(Mesh* m, RenderBuffers *renderBuffers);

void CreateSpringGrid(SimBuffers *buffers,
					Vec3 lower,
					int dx, int dy, int dz,
					float radius,
					int phase,
					float stretchStiffness, float bendStiffness, float shearStiffness,
					Vec3 velocity,
					float invMass);

void CreateTetMesh(SimBuffers *buffers, const char* filename, Vec3 lower, float scale, float stiffness, int phase);

int CreateClusters(Vec3* particles,
					const float* priority,
					int numParticles,
					std::vector<int>& outClusterOffsets,
					std::vector<int>& outClusterIndices,
					std::vector<Vec3>& outClusterPositions,
					float radius,
					float smoothing = 0.0f);

int CreateLinks(const Vec3* particles,
				int numParticles,
				std::vector<int>& outSpringIndices,
				std::vector<float>& outSpringLengths,
				std::vector<float>& outSpringStiffness,
				float radius,
				float stiffness = 1.0f);

void CreateSkinning(const Vec3* vertices,
					int numVertices,
					const Vec3* clusters,
					int numClusters,
					float* outWeights,
					int* outIndices,
					float falloff,
					float maxdist);

void SampleMesh(Mesh* mesh,
				Vec3 lower,
				Vec3 scale,
				float rotation,
				float radius,
				float volumeSampling,
				float surfaceSampling,
				std::vector<Vec3>& outPositions);

/////////////////////////////////////////////////////////////////////////////////////
// functions for adding and resizing shapes
/////////////////////////////////////////////////////////////////////////////////////

void AddBox(SimBuffers *buffers, Vec3 halfEdge = Vec3(2.0f), Vec3 center = Vec3(0.0f), Quat quat = Quat(), bool dynamic = false);

void AddPlinth(SimBuffers *buffers);

void AddSphere(SimBuffers *buffers, float radius, Vec3 position, Quat rotation);

void AddTriangleMesh(SimBuffers *buffers, NvFlexTriangleMeshId mesh, Vec3 translation, Quat rotation, Vec3 scale);

void AddSDF(SimBuffers *buffers, NvFlexDistanceFieldId sdf, Vec3 translation, Quat rotation, float width);

Shape AddCapsule(float radius, float halfHeight, Vec3 position, Quat rotation);

Shape ResizeCapsule(Shape shape, float radius, float halfHeight, Vec3 position, Quat rotation);





