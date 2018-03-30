#include <utilits/utilits.h>

#include <flex/core/mesh.h>
#include <flex/core/voxelize.h>
#include <flex/core/sdf.h>
#include <flex/core/pfm.h>
#include <flex/core/platform.h>
#include <flex/core/convex.h>

#include <controller/render_controller/render_buffer.h>
#include <controller/render_controller/render_gl/shaders.h>
#include <controller/compute_controller/sim_buffers.h>
#include <controller/compute_controller/flex_controller.h>

/////////////////////////////////////////////////////////////
// simple helper functions
///////////////////////////////////////////////////////////

inline int GridIndex(int x, int y, int dx) { return y*dx + x; }

float SampleSDF(const float* sdf, int dim, int x, int y, int z)
{
	assert(x < dim && x >= 0);
	assert(y < dim && y >= 0);
	assert(z < dim && z >= 0);

	return sdf[z*dim*dim + y*dim + x];
}

void CreateSDF(const Mesh* mesh, uint32_t dim, Vec3 lower, Vec3 upper, float* sdf)
{
	if (mesh)
	{
		printf("Begin mesh voxelization\n");

		double startVoxelize = GetSeconds();

		uint32_t* volume = new uint32_t[dim*dim*dim];
		Voxelize((const float*)&mesh->m_positions[0], mesh->m_positions.size(), (const int*)&mesh->m_indices[0], mesh->m_indices.size(), dim, dim, dim, volume, lower, upper);

		printf("End mesh voxelization (%.2fs)\n", (GetSeconds() - startVoxelize));

		printf("Begin SDF gen (fast marching method)\n");

		double startSDF = GetSeconds();

		MakeSDF(volume, dim, dim, dim, sdf);

		printf("End SDF gen (%.2fs)\n", (GetSeconds() - startSDF));

		delete[] volume;
	}
}

NvFlexDistanceFieldId CreateSDF(RenderBuffers *renderBuffers, const char* meshFile, int dim, float margin, float expand)
{
	Mesh* mesh = ImportMesh(meshFile);

	// include small margin to ensure valid gradients near the boundary
	mesh->Normalize(1.0f - margin);
	mesh->Transform(TranslationMatrix(Point3(margin, margin, margin)*0.5f));

	Vec3 lower(0.0f);
	Vec3 upper(1.0f);

	std::string sdfFile = std::string(meshFile, strrchr(meshFile, '.')) + ".pfm";

	PfmImage pfm;
	if (!PfmLoad(sdfFile.c_str(), pfm))
	{
		pfm.m_width = dim;
		pfm.m_height = dim;
		pfm.m_depth = dim;
		pfm.m_data = new float[dim*dim*dim];

		printf("Cooking SDF: %s - dim: %d^3\n", sdfFile.c_str(), dim);

		CreateSDF(mesh, dim, lower, upper, pfm.m_data);

		PfmSave(sdfFile.c_str(), pfm);
	}

	//printf("Loaded SDF, %d\n", pfm.m_width);

	assert(pfm.m_width == pfm.m_height && pfm.m_width == pfm.m_depth);

	// cheap collision offset
	int numVoxels = int(pfm.m_width*pfm.m_height*pfm.m_depth);
	for (int i = 0; i < numVoxels; ++i)
		pfm.m_data[i] += expand;

	FlexController &flexController = FlexController::Instance();

	NvFlexVector<float> field(flexController.GetLib());
	field.assign(pfm.m_data, pfm.m_width*pfm.m_height*pfm.m_depth);
	field.unmap();

	// set up flex collision shape
	NvFlexDistanceFieldId sdf = NvFlexCreateDistanceField(flexController.GetLib());
	NvFlexUpdateDistanceField(flexController.GetLib(), sdf, dim, dim, dim, field.buffer);

	// entry in the collision->render map
	renderBuffers->fields[sdf] = CreateGpuMesh(mesh);

	delete mesh;

	return sdf;
}

// return normal of signed distance field
Vec3 SampleSDFGrad(const float* sdf, int dim, int x, int y, int z)
{
	int x0 = std::max(x - 1, 0);
	int x1 = std::min(x + 1, dim - 1);

	int y0 = std::max(y - 1, 0);
	int y1 = std::min(y + 1, dim - 1);

	int z0 = std::max(z - 1, 0);
	int z1 = std::min(z + 1, dim - 1);

	float dx = (SampleSDF(sdf, dim, x1, y, z) - SampleSDF(sdf, dim, x0, y, z))*(dim*0.5f);
	float dy = (SampleSDF(sdf, dim, x, y1, z) - SampleSDF(sdf, dim, x, y0, z))*(dim*0.5f);
	float dz = (SampleSDF(sdf, dim, x, y, z1) - SampleSDF(sdf, dim, x, y, z0))*(dim*0.5f);

	return Vec3(dx, dy, dz);
}

void GetParticleBounds(SimBuffers *buffers, Vec3& lower, Vec3& upper)
{
	lower = Vec3(FLT_MAX);
	upper = Vec3(-FLT_MAX);

	for (int i = 0; i < buffers->positions.size(); ++i)
	{
		lower = Min(Vec3(buffers->positions.get(i)), lower);
		upper = Max(Vec3(buffers->positions.get(i)), upper);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// functions for calculate
////////////////////////////////////////////////////////////////////////////////////////

// calculates local space positions given a set of particles and rigid indices
void CalculateRigidLocalPositions(const Vec4* restPositions, 
								size_t numRestPositions, 
								const int* offsets, 
								const int* indices, 
								int numRigids, 
								Vec3* localPositions)
{
	// To improve the accuracy of the result, first transform the restPositions to relative coordinates (by finding the mean and subtracting that from all points)
	// Note: If this is not done, one might see ghost forces if the mean of the restPositions is far from the origin.

	// Calculate mean
	Vec3 shapeOffset(0.0f);

	for (size_t i = 0; i < numRestPositions; i++)
	{
		shapeOffset += Vec3(restPositions[i]);
	}

	shapeOffset /= float(numRestPositions);

	size_t count = 0;

	for (int r = 0; r < numRigids; ++r)
	{
		const size_t startIndex = offsets[r];
		const size_t endIndex = offsets[r + 1];

		const size_t n = endIndex - startIndex;

		assert(n);

		Vec3 com;

		for (size_t i = startIndex; i < endIndex; ++i) {
			const size_t r = indices[i];

			// By substracting meshOffset the calculation is done in relative coordinates
			com += Vec3(restPositions[r]) - shapeOffset;
		}

		com /= float(n);

		for (size_t i = startIndex; i < endIndex; ++i)
		{
			const size_t r = indices[i];

			// By substracting meshOffset the calculation is done in relative coordinates
			localPositions[count++] = (Vec3(restPositions[r]) - shapeOffset) - com;
		}
	}
}

float FindMinDistToSet(Vec3 point, FruitVector<Vec4> &bufferOfSet, int startSet, int endSet) {

	Vec3 temp = (point - Vec3(bufferOfSet[startSet]));

	float minDistSqr = temp.x * temp.x + temp.y * temp.y + temp.z * temp.z;
	for (int i = startSet + 1; i < endSet; i++) {
		temp = (point - Vec3(bufferOfSet[i]));
		
		float distSqr = temp.x * temp.x + temp.y * temp.y + temp.z * temp.z;
		if (minDistSqr > distSqr)
			minDistSqr = distSqr;
	}

	return sqrt(minDistSqr);
}

float FindMinDistToSetWithAngle(Vec3 point, Vec3 direction, float max_angle, FruitVector<Vec4> &bufferOfSet, int startSet, int endSet) {

	Vec3 temp = (point - Vec3(bufferOfSet[startSet]));

	float minDistSqr = temp.x * temp.x + temp.y * temp.y + temp.z * temp.z;
	for (int i = startSet + 1; i < endSet; i++) {
		temp = (point - Vec3(bufferOfSet[i]));

		float distSqr = temp.x * temp.x + temp.y * temp.y + temp.z * temp.z;
		if (minDistSqr > distSqr)
		{
			temp = Normalize(temp);
			float angle = acos(Dot3(temp, direction));

			if (angle < max_angle)
				minDistSqr = distSqr;
		}

	}

	return sqrt(minDistSqr);
}

float angleBtwVectors(const Vec3& a, const Vec3& b)
{
	return acos(Dot3(a, b) / Length(a) / Length(b));
}

Vec3 CalculateMean(const Vec3* particles, const int* indices, int numIndices)
{
	Vec3 sum;

	for (int i = 0; i < numIndices; ++i)
		sum += Vec3(particles[indices[i]]);

	if (numIndices)
		return sum / float(numIndices);
	else
		return sum;
}

float CalculateRadius(const Vec3* particles, Vec3 center, const int* indices, int numIndices)
{
	float radiusSq = 0.0f;

	for (int i = 0; i < numIndices; ++i)
	{
		float dSq = LengthSq(Vec3(particles[indices[i]]) - center);
		if (dSq > radiusSq)
			radiusSq = dSq;
	}

	return sqrtf(radiusSq);
}

// calculates the union bounds of all the collision shapes in the scene
void GetShapeBounds(SimBuffers *buffers, Vec3& totalLower, Vec3& totalUpper)
{
	Bounds totalBounds;
	FlexController &flexController = FlexController::Instance();

	for (int i = 0; i < buffers->shapeFlags.size(); ++i)
	{
		NvFlexCollisionGeometry geo = buffers->shapeGeometry[i];

		int type = buffers->shapeFlags[i] & eNvFlexShapeFlagTypeMask;

		Vec3 localLower;
		Vec3 localUpper;

		switch (type)
		{
		case eNvFlexShapeBox:
		{
			localLower = -Vec3(geo.box.halfExtents);
			localUpper = Vec3(geo.box.halfExtents);
			break;
		}
		case eNvFlexShapeSphere:
		{
			localLower = -geo.sphere.radius;
			localUpper = geo.sphere.radius;
			break;
		}
		case eNvFlexShapeCapsule:
		{
			localLower = -Vec3(geo.capsule.halfHeight, 0.0f, 0.0f) - Vec3(geo.capsule.radius);
			localUpper = Vec3(geo.capsule.halfHeight, 0.0f, 0.0f) + Vec3(geo.capsule.radius);
			break;
		}
		case eNvFlexShapeConvexMesh:
		{
			NvFlexGetConvexMeshBounds(flexController.GetLib(), geo.convexMesh.mesh, localLower, localUpper);

			// apply instance scaling
			localLower *= geo.convexMesh.scale;
			localUpper *= geo.convexMesh.scale;
			break;
		}
		case eNvFlexShapeTriangleMesh:
		{
			NvFlexGetTriangleMeshBounds(flexController.GetLib(), geo.triMesh.mesh, localLower, localUpper);

			// apply instance scaling
			localLower *= Vec3(geo.triMesh.scale);
			localUpper *= Vec3(geo.triMesh.scale);
			break;
		}
		case eNvFlexShapeSDF:
		{
			localLower = 0.0f;
			localUpper = geo.sdf.scale;
			break;
		}
		};

		// transform local bounds to world space
		Vec3 worldLower, worldUpper;
		TransformBounds(localLower, localUpper, Vec3(buffers->shapePositions[i]), buffers->shapeRotations[i], 1.0f, worldLower, worldUpper);

		totalBounds = Union(totalBounds, Bounds(worldLower, worldUpper));
	}

	totalLower = totalBounds.lower;
	totalUpper = totalBounds.upper;
}

//////////////////////////////////////////////////////////////
// functions for create
////////////////////////////////////////////////////////

void CreateRigidCapsule(SimBuffers &buffers, RenderBuffers &renderBuffers,
						float radius, float halfLength, int slices, int segments,
						Vec3 lower, Vec3 scale, float rotation, float samplingMesh, 
						Vec3 velocity, float mass, int phase) {
	
	Mesh *mesh = CreateCapsule(slices, segments, radius, halfLength);
	mesh->m_colours = std::vector<Colour>(mesh->m_positions.size());

	CreateParticleShape(&buffers, &renderBuffers, mesh, 
						lower, scale, rotation, 
						samplingMesh, velocity, mass, 
						true, 1.5, NvFlexMakePhase(phase, 0), true);

	buffers.numParticles = buffers.positions.size();
	buffers.maxParticles = buffers.numParticles + buffers.numExtraParticles * buffers.numExtraMultiplier;

	//size_t indexCapsule = buffers.restPositions.size();

	for (int i = buffers.restPositions.size(); i < buffers.positions.size(); i++) {
		buffers.restPositions.push_back(buffers.positions[i]);
		buffers.activeIndices.push_back(i);
	}

	buffers.BuildConstraints();

	// need refactoring
	for (int i = renderBuffers.meshRestPositions.size(); i < renderBuffers.mesh->m_positions.size(); i++) {
		renderBuffers.meshRestPositions.push_back(renderBuffers.mesh->m_positions[i]);
	}

}

void CreateParticleGrid(SimBuffers *buffer,
						Vec3 lower, 
						int dimx, int dimy, int dimz, 
						float radius, 
						Vec3 velocity, 
						float invMass, 
						bool rigid, float rigidStiffness, 
						int phase, float jitter)
{
	if (rigid && buffer->rigidIndices.empty())
		buffer->rigidOffsets.push_back(0);

	for (int x = 0; x < dimx; ++x)
	{
		for (int y = 0; y < dimy; ++y)
		{
			for (int z = 0; z < dimz; ++z)
			{
				if (rigid)
					buffer->rigidIndices.push_back(int(buffer->positions.size()));

				Vec3 position = lower + Vec3(float(x), float(y), float(z))*radius + RandomUnitVector()*jitter;

				buffer->positions.push_back(Vec4(position.x, position.y, position.z, invMass));
				buffer->velocities.push_back(velocity);
				buffer->phases.push_back(phase);
			}
		}
	}

	if (rigid)
	{
		buffer->rigidCoefficients.push_back(rigidStiffness);
		buffer->rigidOffsets.push_back(int(buffer->rigidIndices.size()));
	}
}

void CreateParticleSphere(SimBuffers *buffers, 
						  Vec3 center, 
						  int dim, float radius, 
						  Vec3 velocity, 
						  float invMass, 
						  bool rigid, float rigidStiffness, 
						  int phase, 
						  float jitter)
{
	if (rigid && buffers->rigidIndices.empty())
		buffers->rigidOffsets.push_back(0);

	for (int x = 0; x <= dim; ++x)
	{
		for (int y = 0; y <= dim; ++y)
		{
			for (int z = 0; z <= dim; ++z)
			{
				float sx = x - dim*0.5f;
				float sy = y - dim*0.5f;
				float sz = z - dim*0.5f;

				if (sx*sx + sy*sy + sz*sz <= float(dim*dim)*0.25f)
				{
					if (rigid)
						buffers->rigidIndices.push_back(int(buffers->positions.size()));

					Vec3 position = center + radius*Vec3(sx, sy, sz) + RandomUnitVector()*jitter;

					buffers->positions.push_back(Vec4(position.x, position.y, position.z, invMass));
					buffers->velocities.push_back(velocity);
					buffers->phases.push_back(phase);
				}
			}
		}
	}

	if (rigid)
	{
		buffers->rigidCoefficients.push_back(rigidStiffness);
		buffers->rigidOffsets.push_back(int(buffers->rigidIndices.size()));
	}
}


void CreateSpring(SimBuffers *buffers, int i, int j, float stiffness, float give) {
	buffers->springIndices.push_back(i);
	buffers->springIndices.push_back(j);
	buffers->springLengths.push_back((1.0f + give)*Length(Vec3(buffers->positions.get(i)) - Vec3(buffers->positions.get(j))));
	buffers->springStiffness.push_back(stiffness);
}

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
						float jitter,
						Vec3 skinOffset,
						float skinExpand,
						Vec4 color,
						float springStiffness)
{
	if (rigid && buffers->rigidIndices.empty())
		buffers->rigidOffsets.push_back(0);

	if (!srcMesh)
		return;

	// duplicate mesh
	Mesh mesh;
	mesh.AddMesh(*srcMesh);

	int startIndex = int(buffers->positions.size());

	{
		mesh.Transform(RotationMatrix(rotation, Vec3(0.0f, 1.0f, 0.0f)));

		Vec3 meshLower, meshUpper;
		mesh.GetBounds(meshLower, meshUpper);

		Vec3 edges = meshUpper - meshLower;
		float maxEdge = std::max(std::max(edges.x, edges.y), edges.z);

		// put mesh at the origin and scale to specified size
		Matrix44 xform = ScaleMatrix(scale / maxEdge)*TranslationMatrix(Point3(-meshLower));

		mesh.Transform(xform);
		mesh.GetBounds(meshLower, meshUpper);

		// recompute expanded edges
		edges = meshUpper - meshLower;
		maxEdge = std::max(std::max(edges.x, edges.y), edges.z);

		// tweak spacing to avoid edge cases for particles laying on the boundary
		// just covers the case where an edge is a whole multiple of the spacing.
		float spacingEps = spacing*(1.0f - 1e-4f);

		// make sure to have at least one particle in each dimension
		int dx, dy, dz;
		dx = spacing > edges.x ? 1 : int(edges.x / spacingEps);
		dy = spacing > edges.y ? 1 : int(edges.y / spacingEps);
		dz = spacing > edges.z ? 1 : int(edges.z / spacingEps);

		int maxDim = std::max(std::max(dx, dy), dz);

		// expand border by two voxels to ensure adequate sampling at edges
		meshLower -= 2.0f*Vec3(spacing);
		meshUpper += 2.0f*Vec3(spacing);
		maxDim += 4;

		std::vector<uint32_t> voxels(maxDim*maxDim*maxDim);

		// we shift the voxelization bounds so that the voxel centers
		// lie symmetrically to the center of the object. this reduces the 
		// chance of missing features, and also better aligns the particles
		// with the mesh
		Vec3 meshOffset;
		meshOffset.x = 0.5f * (spacing - (edges.x - (dx - 1)*spacing));
		meshOffset.y = 0.5f * (spacing - (edges.y - (dy - 1)*spacing));
		meshOffset.z = 0.5f * (spacing - (edges.z - (dz - 1)*spacing));
		meshLower -= meshOffset;

		//Voxelize(*mesh, dx, dy, dz, &voxels[0], meshLower - Vec3(spacing*0.05f) , meshLower + Vec3(maxDim*spacing) + Vec3(spacing*0.05f));
		Voxelize((const float*)&mesh.m_positions[0], mesh.m_positions.size(), (const int*)&mesh.m_indices[0], mesh.m_indices.size(), maxDim, maxDim, maxDim, &voxels[0], meshLower, meshLower + Vec3(maxDim*spacing));

		std::vector<int> indices(maxDim*maxDim*maxDim);
		std::vector<float> sdf(maxDim*maxDim*maxDim);
		MakeSDF(&voxels[0], maxDim, maxDim, maxDim, &sdf[0]);

		for (int x = 0; x < maxDim; ++x)
		{
			for (int y = 0; y < maxDim; ++y)
			{
				for (int z = 0; z < maxDim; ++z)
				{
					const int index = z*maxDim*maxDim + y*maxDim + x;

					// if voxel is marked as occupied the add a particle
					if (voxels[index])
					{
						if (rigid)
							buffers->rigidIndices.push_back(int(buffers->positions.size()));

						Vec3 position = lower + meshLower + spacing*Vec3(float(x) + 0.5f, float(y) + 0.5f, float(z) + 0.5f) + RandomUnitVector()*jitter;

						// normalize the sdf value and transform to world scale
						Vec3 n = SafeNormalize(SampleSDFGrad(&sdf[0], maxDim, x, y, z));
						float d = sdf[index] * maxEdge;

						if (rigid)
							buffers->rigidLocalNormals.push_back(Vec4(n, d));

						// track which particles are in which cells
						indices[index] = buffers->positions.size();

						buffers->positions.push_back(Vec4(position.x, position.y, position.z, invMass));
						buffers->velocities.push_back(velocity);
						buffers->phases.push_back(phase);
					}
				}
			}
		}
		mesh.Transform(ScaleMatrix(1.0f + skinExpand)*TranslationMatrix(Point3(-0.5f*(meshUpper + meshLower))));
		mesh.Transform(TranslationMatrix(Point3(lower + 0.5f*(meshUpper + meshLower))));


		if (springStiffness > 0.0f)
		{
			// construct cross link springs to occupied cells
			for (int x = 0; x < maxDim; ++x)
			{
				for (int y = 0; y < maxDim; ++y)
				{
					for (int z = 0; z < maxDim; ++z)
					{
						const int centerCell = z*maxDim*maxDim + y*maxDim + x;

						// if voxel is marked as occupied the add a particle
						if (voxels[centerCell])
						{
							const int width = 1;

							// create springs to all the neighbors within the width
							for (int i = x - width; i <= x + width; ++i)
							{
								for (int j = y - width; j <= y + width; ++j)
								{
									for (int k = z - width; k <= z + width; ++k)
									{
										const int neighborCell = k*maxDim*maxDim + j*maxDim + i;

										if (neighborCell > 0 && neighborCell < int(voxels.size()) && voxels[neighborCell] && neighborCell != centerCell)
										{
											CreateSpring(buffers, indices[neighborCell], indices[centerCell], springStiffness);
										}
									}
								}
							}
						}
					}
				}
			}
		}

	}


	if (skin)
	{
		buffers->rigidMeshSize.push_back(mesh.GetNumVertices());

		int startVertex = 0;

		if (!renderBuffers->mesh)
			renderBuffers->mesh = new Mesh();

		// append to mesh
		startVertex = renderBuffers->mesh->GetNumVertices();

		renderBuffers->mesh->Transform(TranslationMatrix(Point3(skinOffset)));
		renderBuffers->mesh->AddMesh(mesh);

		const Colour colors[7] =
		{
			Colour(0.0f, 0.5f, 1.0f),
			Colour(0.797f, 0.354f, 0.000f),
			Colour(0.000f, 0.349f, 0.173f),
			Colour(0.875f, 0.782f, 0.051f),
			Colour(0.01f, 0.170f, 0.453f),
			Colour(0.673f, 0.111f, 0.000f),
			Colour(0.612f, 0.194f, 0.394f)
		};

		for (uint32_t i = startVertex; i < renderBuffers->mesh->GetNumVertices(); ++i)
		{
			int indices[4] = { -1, -1, -1, -1 };
			float distances[4] = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };

			if (LengthSq(color) == 0.0f)
				renderBuffers->mesh->m_colours[i] = 1.25f*colors[phase % 7];
			else
				renderBuffers->mesh->m_colours[i] = Colour(color);

			// find closest n particles
			for (int j = startIndex; j < buffers->positions.size(); ++j)
			{
				float dSq = LengthSq(Vec3(renderBuffers->mesh->m_positions[i]) - Vec3(buffers->positions.get(j)));

				// insertion sort
				int w = 0;
				for (; w < 4; ++w)
					if (dSq < distances[w])
						break;

				if (w < 4)
				{
					// shuffle down
					for (int s = 3; s > w; --s)
					{
						indices[s] = indices[s - 1];
						distances[s] = distances[s - 1];
					}

					distances[w] = dSq;
					indices[w] = int(j);
				}
			}

			// weight particles according to distance
			float wSum = 0.0f;

			for (int w = 0; w < 4; ++w)
			{
				// convert to inverse distance
				distances[w] = 1.0f / (0.1f + powf(distances[w], .125f));

				wSum += distances[w];

			}

			float weights[4];
			for (int w = 0; w < 4; ++w)
				weights[w] = distances[w] / wSum;

			for (int j = 0; j < 4; ++j)
			{
				renderBuffers->meshSkinIndices.push_back(indices[j]);
				renderBuffers->meshSkinWeights.push_back(weights[j]);
			}
		}
	}

	if (rigid)
	{
		buffers->rigidCoefficients.push_back(rigidStiffness);
		buffers->rigidOffsets.push_back(int(buffers->rigidIndices.size()));
	}
}

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
						float jitter, 
						Vec3 skinOffset, 
						float skinExpand, 
						Vec4 color, 
						float springStiffness)
{
	Mesh* mesh = ImportMesh(filename);
	if (mesh)
		CreateParticleShape(buffers, 
							renderBuffers,
							mesh, 
							lower, scale, 
							rotation, spacing, 
							velocity, 
							invMass, 
							rigid, rigidStiffness, 
							phase, skin, 
							jitter, 
							skinOffset, 
							skinExpand, 
							color,
							springStiffness);

	delete mesh;
}

void CreateRandomBody(SimBuffers *buffers,
					int numPlanes, 
					Vec3 position, 
					float minDist, 
					float maxDist, 
					Vec3 axis, 
					float angle, 
					float invMass, 
					int phase, 
					float stiffness)
{
	// 12-kdop
	const Vec3 directions[] = {
		Vec3(1.0f, 0.0f, 0.0f),
		Vec3(0.0f, 1.0f, 0.0f),
		Vec3(0.0f, 0.0f, 1.0f),
		Vec3(-1.0f, 0.0f, 0.0f),
		Vec3(0.0f, -1.0f, 0.0f),
		Vec3(0.0f, 0.0f, -1.0f),
		Vec3(1.0f, 1.0f, 0.0f),
		Vec3(-1.0f, -1.0f, 0.0f),
		Vec3(1.0f, 0.0f, 1.0f),
		Vec3(-1.0f, 0.0f, -1.0f),
		Vec3(0.0f, 1.0f, 1.0f),
		Vec3(0.0f, -1.0f, -1.0f),
	};

	numPlanes = std::max(4, numPlanes);

	std::vector<Vec4> planes;

	// create a box
	for (int i = 0; i < numPlanes; ++i)
	{
		// pick random dir and distance
		Vec3 dir = Normalize(directions[i]);//RandomUnitVector();
		float dist = Randf(minDist, maxDist);

		planes.push_back(Vec4(dir.x, dir.y, dir.z, -dist));
	}

	// set aabbs
	ConvexMeshBuilder builder(&planes[0]);
	builder(numPlanes);

	int startIndex = int(buffers->positions.size());

	for (size_t v = 0; v < builder.mVertices.size(); ++v)
	{
		Quat q = QuatFromAxisAngle(axis, angle);
		Vec3 p = rotate(Vec3(q), q.w, builder.mVertices[v]) + position;

		buffers->positions.push_back(Vec4(p.x, p.y, p.z, invMass));
		buffers->velocities.push_back(0.0f);
		buffers->phases.push_back(phase);

		// add spring to all verts with higher index
		for (size_t i = v + 1; i < builder.mVertices.size(); ++i)
		{
			int a = startIndex + int(v);
			int b = startIndex + int(i);

			buffers->springIndices.push_back(a);
			buffers->springIndices.push_back(b);
			buffers->springLengths.push_back(Length(builder.mVertices[v] - builder.mVertices[i]));
			buffers->springStiffness.push_back(stiffness);

		}
	}

	for (size_t t = 0; t < builder.mIndices.size(); ++t)
		buffers->triangles.push_back(startIndex + builder.mIndices[t]);

	// lazy
	buffers->triangleNormals.resize(buffers->triangleNormals.size() + builder.mIndices.size() / 3, Vec3(0.0f));
}

NvFlexTriangleMeshId CreateTriangleMesh(Mesh* m, RenderBuffers *renderBuffers) {
	if (!m)
		return 0;

	FlexController &flexController = FlexController::Instance();

	Vec3 lower, upper;
	m->GetBounds(lower, upper);

	NvFlexVector<Vec3> positions(flexController.GetLib());
	NvFlexVector<int> indices(flexController.GetLib());

	positions.assign((Vec3*)&m->m_positions[0], m->m_positions.size());
	indices.assign((int*)&m->m_indices[0], m->m_indices.size());

	positions.unmap();
	indices.unmap();

	NvFlexTriangleMeshId flexMesh = NvFlexCreateTriangleMesh(flexController.GetLib());
	NvFlexUpdateTriangleMesh(flexController.GetLib(), 
							flexMesh, 
							positions.buffer, 
							indices.buffer, 
							m->GetNumVertices(), 
							m->GetNumFaces(), 
							(float*)&lower, 
							(float*)&upper);

	// entry in the collision->render map
	renderBuffers->meshes[flexMesh] = CreateGpuMesh(m);

	return flexMesh;
}

void CreateSpringGrid(SimBuffers *buffers, 
					Vec3 lower, 
					int dx, int dy, int dz, 
					float radius, 
					int phase, 
					float stretchStiffness, float bendStiffness, float shearStiffness, 
					Vec3 velocity, 
					float invMass)
{
	int baseIndex = int(buffers->positions.size());

	for (int z = 0; z < dz; ++z)
	{
		for (int y = 0; y < dy; ++y)
		{
			for (int x = 0; x < dx; ++x)
			{
				Vec3 position = lower + radius*Vec3(float(x), float(z), float(y));

				buffers->positions.push_back(Vec4(position.x, position.y, position.z, invMass));
				buffers->velocities.push_back(velocity);
				buffers->phases.push_back(phase);

				if (x > 0 && y > 0)
				{
					buffers->triangles.push_back(baseIndex + GridIndex(x - 1, y - 1, dx));
					buffers->triangles.push_back(baseIndex + GridIndex(x, y - 1, dx));
					buffers->triangles.push_back(baseIndex + GridIndex(x, y, dx));

					buffers->triangles.push_back(baseIndex + GridIndex(x - 1, y - 1, dx));
					buffers->triangles.push_back(baseIndex + GridIndex(x, y, dx));
					buffers->triangles.push_back(baseIndex + GridIndex(x - 1, y, dx));

					buffers->triangleNormals.push_back(Vec3(0.0f, 1.0f, 0.0f));
					buffers->triangleNormals.push_back(Vec3(0.0f, 1.0f, 0.0f));
				}
			}
		}
	}

	// horizontal
	for (int y = 0; y < dy; ++y)
	{
		for (int x = 0; x < dx; ++x)
		{
			int index0 = y*dx + x;

			if (x > 0)
			{
				int index1 = y*dx + x - 1;
				CreateSpring(buffers, baseIndex + index0, baseIndex + index1, stretchStiffness);
			}

			if (x > 1)
			{
				int index2 = y*dx + x - 2;
				CreateSpring(buffers, baseIndex + index0, baseIndex + index2, bendStiffness);
			}

			if (y > 0 && x < dx - 1)
			{
				int indexDiag = (y - 1)*dx + x + 1;
				CreateSpring(buffers, baseIndex + index0, baseIndex + indexDiag, shearStiffness);
			}

			if (y > 0 && x > 0)
			{
				int indexDiag = (y - 1)*dx + x - 1;
				CreateSpring(buffers, baseIndex + index0, baseIndex + indexDiag, shearStiffness);
			}
		}
	}

	// vertical
	for (int x = 0; x < dx; ++x)
	{
		for (int y = 0; y < dy; ++y)
		{
			int index0 = y*dx + x;

			if (y > 0)
			{
				int index1 = (y - 1)*dx + x;
				CreateSpring(buffers, baseIndex + index0, baseIndex + index1, stretchStiffness);
			}

			if (y > 1)
			{
				int index2 = (y - 2)*dx + x;
				CreateSpring(buffers, baseIndex + index0, baseIndex + index2, bendStiffness);
			}
		}
	}
}


void CreateTetMesh(SimBuffers *buffers, const char* filename, Vec3 lower, float scale, float stiffness, int phase)
{
	FILE* f = fopen(filename, "r");

	char line[2048];

	if (f)
	{
		typedef std::map<TriKey, int> TriMap;
		TriMap triCount;

		const int vertOffset = buffers->positions.size();

		Vec3 meshLower(FLT_MAX);
		Vec3 meshUpper(-FLT_MAX);

		bool firstTet = true;

		while (!feof(f))
		{
			if (fgets(line, 2048, f))
			{
				switch (line[0])
				{
				case '#':
					break;
				case 'v':
				{
					Vec3 pos;
					sscanf(line, "v %f %f %f", &pos.x, &pos.y, &pos.z);

					buffers->positions.push_back(Vec4(pos.x, pos.y, pos.z, 1.0f));
					buffers->velocities.push_back(0.0f);
					buffers->phases.push_back(phase);

					meshLower = Min(pos, meshLower);
					meshUpper = Max(pos, meshUpper);
					break;
				}
				case 't':
				{
					if (firstTet)
					{
						Vec3 edges = meshUpper - meshLower;
						float maxEdge = std::max(edges.x, std::max(edges.y, edges.z));

						// normalize positions
						for (int i = vertOffset; i < int(buffers->positions.size()); ++i)
						{
							Vec3 p = lower + (Vec3(buffers->positions.get(i)) - meshLower)*scale / maxEdge;
							buffers->positions.set(i, Vec4(p, buffers->positions.get(i).w));
						}

						firstTet = false;
					}

					int indices[4];
					sscanf(line, "t %d %d %d %d", &indices[0], &indices[1], &indices[2], &indices[3]);

					indices[0] += vertOffset;
					indices[1] += vertOffset;
					indices[2] += vertOffset;
					indices[3] += vertOffset;

					CreateSpring(buffers, indices[0], indices[1], stiffness);
					CreateSpring(buffers, indices[0], indices[2], stiffness);
					CreateSpring(buffers, indices[0], indices[3], stiffness);

					CreateSpring(buffers, indices[1], indices[2], stiffness);
					CreateSpring(buffers, indices[1], indices[3], stiffness);
					CreateSpring(buffers, indices[2], indices[3], stiffness);

					TriKey k1(indices[0], indices[2], indices[1]);
					triCount[k1] += 1;

					TriKey k2(indices[1], indices[2], indices[3]);
					triCount[k2] += 1;

					TriKey k3(indices[0], indices[1], indices[3]);
					triCount[k3] += 1;

					TriKey k4(indices[0], indices[3], indices[2]);
					triCount[k4] += 1;

					break;
				}
				}
			}
		}

		for (TriMap::iterator iter = triCount.begin(); iter != triCount.end(); ++iter)
		{
			TriKey key = iter->first;

			// only output faces that are referenced by one tet (open faces)
			if (iter->second == 1)
			{
				buffers->triangles.push_back(key.orig[0]);
				buffers->triangles.push_back(key.orig[1]);
				buffers->triangles.push_back(key.orig[2]);
				buffers->triangleNormals.push_back(0.0f);
			}
		}


		fclose(f);
	}
}

int CreateClusters(Vec3* particles, 
	const float* priority, 
	int numParticles, 
	std::vector<int>& outClusterOffsets, 
	std::vector<int>& outClusterIndices, 
	std::vector<Vec3>& outClusterPositions, 
	float radius, 
	float smoothing)
{
	std::vector<Seed> seeds;
	std::vector<Cluster> clusters;

	// flags a particle as belonging to at least one cluster
	std::vector<bool> used(numParticles, false);

	// initialize seeds
	for (int i = 0; i < numParticles; ++i)
	{
		Seed s;
		s.index = i;
		s.priority = priority[i];

		seeds.push_back(s);
	}

	std::stable_sort(seeds.begin(), seeds.end());

	while (seeds.size())
	{
		// pick highest unused particle from the seeds list
		Seed seed = seeds.back();
		seeds.pop_back();

		if (!used[seed.index])
		{
			Cluster c;

			const float radiusSq = sqr(radius);

			// push all neighbors within radius
			for (int p = 0; p < numParticles; ++p)
			{
				float dSq = LengthSq(Vec3(particles[seed.index]) - Vec3(particles[p]));
				if (dSq <= radiusSq)
				{
					c.indices.push_back(p);

					used[p] = true;
				}
			}

			c.mean = CalculateMean(particles, &c.indices[0], c.indices.size());

			clusters.push_back(c);
		}
	}

	if (smoothing > 0.0f)
	{
		// expand clusters by smoothing radius
		float radiusSmoothSq = sqr(smoothing);

		for (int i = 0; i < int(clusters.size()); ++i)
		{
			Cluster& c = clusters[i];

			// clear cluster indices
			c.indices.resize(0);

			// push all neighbors within radius
			for (int p = 0; p < numParticles; ++p)
			{
				float dSq = LengthSq(c.mean - Vec3(particles[p]));
				if (dSq <= radiusSmoothSq)
					c.indices.push_back(p);
			}

			c.mean = CalculateMean(particles, &c.indices[0], c.indices.size());
		}
	}

	// write out cluster indices
	int count = 0;

	//outClusterOffsets.push_back(0);

	for (int c = 0; c < int(clusters.size()); ++c)
	{
		const Cluster& cluster = clusters[c];

		const int clusterSize = int(cluster.indices.size());

		// skip empty clusters
		if (clusterSize)
		{
			// write cluster indices
			for (int i = 0; i < clusterSize; ++i)
				outClusterIndices.push_back(cluster.indices[i]);

			// write cluster offset
			outClusterOffsets.push_back(outClusterIndices.size());

			// write center
			outClusterPositions.push_back(cluster.mean);

			++count;
		}
	}

	return count;
}

// creates distance constraints between particles within some distance
int CreateLinks(const Vec3* particles, 
				int numParticles, 
				std::vector<int>& outSpringIndices, 
				std::vector<float>& outSpringLengths, 
				std::vector<float>& outSpringStiffness, 
				float radius, 
				float stiffness)
{
	const float radiusSq = sqr(radius);

	int count = 0;

	for (int i = 0; i < numParticles; ++i)
	{
		for (int j = i + 1; j < numParticles; ++j)
		{
			float dSq = LengthSq(Vec3(particles[i]) - Vec3(particles[j]));

			if (dSq < radiusSq)
			{
				outSpringIndices.push_back(i);
				outSpringIndices.push_back(j);
				outSpringLengths.push_back(sqrtf(dSq));
				outSpringStiffness.push_back(stiffness);

				++count;
			}
		}
	}

	return count;
}

void CreateSkinning(const Vec3* vertices, 
					int numVertices, 
					const Vec3* clusters, 
					int numClusters, 
					float* outWeights, 
					int* outIndices, 
					float falloff, 
					float maxdist)
{
	const int maxBones = 4;

	// for each vertex, find the closest n clusters
	for (int i = 0; i < numVertices; ++i)
	{
		int indices[4] = { -1, -1, -1, -1 };
		float distances[4] = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };
		float weights[maxBones];

		for (int c = 0; c < numClusters; ++c)
		{
			float dSq = LengthSq(vertices[i] - clusters[c]);

			// insertion sort
			int w = 0;
			for (; w < maxBones; ++w)
				if (dSq < distances[w])
					break;

			if (w < maxBones)
			{
				// shuffle down
				for (int s = maxBones - 1; s > w; --s)
				{
					indices[s] = indices[s - 1];
					distances[s] = distances[s - 1];
				}

				distances[w] = dSq;
				indices[w] = c;
			}
		}

		// weight particles according to distance
		float wSum = 0.0f;

		for (int w = 0; w < maxBones; ++w)
		{
			if (distances[w] > sqr(maxdist))
			{
				// clamp bones over a given distance to zero
				weights[w] = 0.0f;
			}
			else
			{
				// weight falls off inversely with distance
				weights[w] = 1.0f / (powf(distances[w], falloff) + 0.0001f);
			}

			wSum += weights[w];
		}

		if (wSum == 0.0f)
		{
			// if all weights are zero then just 
			// rigidly skin to the closest bone
			weights[0] = 1.0f;
		}
		else
		{
			// normalize weights
			for (int w = 0; w < maxBones; ++w)
			{
				weights[w] = weights[w] / wSum;
			}
		}

		// output
		for (int j = 0; j < maxBones; ++j)
		{
			outWeights[i*maxBones + j] = weights[j];
			outIndices[i*maxBones + j] = indices[j];
		}
	}
}

void SampleMesh(Mesh* mesh, 
				Vec3 lower, 
				Vec3 scale, 
				float rotation, 
				float radius, 
				float volumeSampling, 
				float surfaceSampling, 
				std::vector<Vec3>& outPositions)
{
	if (!mesh)
		return;

	mesh->Transform(RotationMatrix(rotation, Vec3(0.0f, 1.0f, 0.0f)));

	Vec3 meshLower, meshUpper;
	mesh->GetBounds(meshLower, meshUpper);

	Vec3 edges = meshUpper - meshLower;
	float maxEdge = std::max(std::max(edges.x, edges.y), edges.z);

	// put mesh at the origin and scale to specified size
	Matrix44 xform = ScaleMatrix(scale / maxEdge)*TranslationMatrix(Point3(-meshLower));

	mesh->Transform(xform);
	mesh->GetBounds(meshLower, meshUpper);

	std::vector<Vec3> samples;

	if (volumeSampling > 0.0f)
	{
		// recompute expanded edges
		edges = meshUpper - meshLower;
		maxEdge = std::max(std::max(edges.x, edges.y), edges.z);

		// use a higher resolution voxelization as a basis for the particle decomposition
		float spacing = radius / volumeSampling;

		// tweak spacing to avoid edge cases for particles laying on the boundary
		// just covers the case where an edge is a whole multiple of the spacing.
		float spacingEps = spacing*(1.0f - 1e-4f);

		// make sure to have at least one particle in each dimension
		int dx, dy, dz;
		dx = spacing > edges.x ? 1 : int(edges.x / spacingEps);
		dy = spacing > edges.y ? 1 : int(edges.y / spacingEps);
		dz = spacing > edges.z ? 1 : int(edges.z / spacingEps);

		int maxDim = std::max(std::max(dx, dy), dz);

		// expand border by two voxels to ensure adequate sampling at edges
		meshLower -= 2.0f*Vec3(spacing);
		meshUpper += 2.0f*Vec3(spacing);
		maxDim += 4;

		std::vector<uint32_t> voxels(maxDim*maxDim*maxDim);

		// we shift the voxelization bounds so that the voxel centers
		// lie symmetrically to the center of the object. this reduces the 
		// chance of missing features, and also better aligns the particles
		// with the mesh
		Vec3 meshOffset;
		meshOffset.x = 0.5f * (spacing - (edges.x - (dx - 1)*spacing));
		meshOffset.y = 0.5f * (spacing - (edges.y - (dy - 1)*spacing));
		meshOffset.z = 0.5f * (spacing - (edges.z - (dz - 1)*spacing));
		meshLower -= meshOffset;

		//Voxelize(*mesh, dx, dy, dz, &voxels[0], meshLower - Vec3(spacing*0.05f) , meshLower + Vec3(maxDim*spacing) + Vec3(spacing*0.05f));
		Voxelize((const float*)&mesh->m_positions[0], mesh->m_positions.size(), (const int*)&mesh->m_indices[0], mesh->m_indices.size(), maxDim, maxDim, maxDim, &voxels[0], meshLower, meshLower + Vec3(maxDim*spacing));

		// sample interior
		for (int x = 0; x < maxDim; ++x)
		{
			for (int y = 0; y < maxDim; ++y)
			{
				for (int z = 0; z < maxDim; ++z)
				{
					const int index = z*maxDim*maxDim + y*maxDim + x;

					// if voxel is marked as occupied the add a particle
					if (voxels[index])
					{
						Vec3 position = lower + meshLower + spacing*Vec3(float(x) + 0.5f, float(y) + 0.5f, float(z) + 0.5f);

						// normalize the sdf value and transform to world scale
						samples.push_back(position);
					}
				}
			}
		}
	}

	// move back
	mesh->Transform(ScaleMatrix(1.0f)*TranslationMatrix(Point3(-0.5f*(meshUpper + meshLower))));
	mesh->Transform(TranslationMatrix(Point3(lower + 0.5f*(meshUpper + meshLower))));

	if (surfaceSampling > 0.0f)
	{
		// sample vertices
		for (int i = 0; i < int(mesh->m_positions.size()); ++i)
			samples.push_back(Vec3(mesh->m_positions[i]));

		// random surface sampling
		if (1)
		{
			for (int i = 0; i < 50000; ++i)
			{
				int t = Rand() % mesh->GetNumFaces();
				float u = Randf();
				float v = Randf()*(1.0f - u);
				float w = 1.0f - u - v;

				int a = mesh->m_indices[t * 3 + 0];
				int b = mesh->m_indices[t * 3 + 1];
				int c = mesh->m_indices[t * 3 + 2];

				Point3 pt = mesh->m_positions[a] * u + mesh->m_positions[b] * v + mesh->m_positions[c] * w;
				Vec3 p(pt.x, pt.y, pt.z);

				samples.push_back(p);
			}
		}
	}

	std::vector<int> clusterIndices;
	std::vector<int> clusterOffsets;
	std::vector<Vec3> clusterPositions; //-V808
	std::vector<float> priority(samples.size());

	CreateClusters(&samples[0], &priority[0], samples.size(), clusterOffsets, clusterIndices, outPositions, radius);

}

/////////////////////////////////////////////////////////////////////////////////////
// functions for adding and resizing shapes
/////////////////////////////////////////////////////////////////////////////////////

void AddBox(SimBuffers *buffers, Vec3 halfEdge, Vec3 center, Quat quat, bool dynamic) {
	// transform
	buffers->shapePositions.push_back(Vec4(center.x, center.y, center.z, 0.0f));
	buffers->shapeRotations.push_back(quat);

	buffers->shapePrevPositions.push_back(buffers->shapePositions.back());
	buffers->shapePrevRotations.push_back(buffers->shapeRotations.back());

	NvFlexCollisionGeometry geo;
	geo.box.halfExtents[0] = halfEdge.x;
	geo.box.halfExtents[1] = halfEdge.y;
	geo.box.halfExtents[2] = halfEdge.z;

	buffers->shapeGeometry.push_back(geo);
	buffers->shapeFlags.push_back(NvFlexMakeShapeFlags(eNvFlexShapeBox, dynamic));
}

void AddSphere(SimBuffers *buffers, float radius, Vec3 position, Quat rotation)
{
	NvFlexCollisionGeometry geo;
	geo.sphere.radius = radius;
	buffers->shapeGeometry.push_back(geo);

	buffers->shapePositions.push_back(Vec4(position, 0.0f));
	buffers->shapeRotations.push_back(rotation);

	buffers->shapePrevPositions.push_back(buffers->shapePositions.back());
	buffers->shapePrevRotations.push_back(buffers->shapeRotations.back());

	int flags = NvFlexMakeShapeFlags(eNvFlexShapeSphere, false);
	buffers->shapeFlags.push_back(flags);
}

// helper that creates a plinth whose center matches the particle bounds
void AddPlinth(SimBuffers *buffers)
{
	Vec3 lower, upper;
	GetParticleBounds(buffers, lower, upper);

	Vec3 center = (lower + upper)*0.5f;
	center.y = 0.5f;

	AddBox(buffers, Vec3(2.0f, 0.5f, 2.0f), center);
}

void AddTriangleMesh(SimBuffers *buffers, NvFlexTriangleMeshId mesh, Vec3 translation, Quat rotation, Vec3 scale)
{
	Vec3 lower, upper;
	NvFlexGetTriangleMeshBounds(FlexController::Instance().GetLib(), mesh, lower, upper);

	NvFlexCollisionGeometry geo;
	geo.triMesh.mesh = mesh;
	geo.triMesh.scale[0] = scale.x;
	geo.triMesh.scale[1] = scale.y;
	geo.triMesh.scale[2] = scale.z;

	buffers->shapePositions.push_back(Vec4(translation, 0.0f));
	buffers->shapeRotations.push_back(Quat(rotation));
	buffers->shapePrevPositions.push_back(Vec4(translation, 0.0f));
	buffers->shapePrevRotations.push_back(Quat(rotation));
	buffers->shapeGeometry.push_back((NvFlexCollisionGeometry&)geo);
	buffers->shapeFlags.push_back(NvFlexMakeShapeFlags(eNvFlexShapeTriangleMesh, false));
}

void AddSDF(SimBuffers *buffers, NvFlexDistanceFieldId sdf, Vec3 translation, Quat rotation, float width)
{
	NvFlexCollisionGeometry geo;
	geo.sdf.field = sdf;
	geo.sdf.scale = width;

	buffers->shapePositions.push_back(Vec4(translation, 0.0f));
	buffers->shapeRotations.push_back(Quat(rotation));
	buffers->shapePrevPositions.push_back(Vec4(translation, 0.0f));
	buffers->shapePrevRotations.push_back(Quat(rotation));
	buffers->shapeGeometry.push_back((NvFlexCollisionGeometry&)geo);
	buffers->shapeFlags.push_back(NvFlexMakeShapeFlags(eNvFlexShapeSDF, false));
}

Shape AddCapsule(float radius, float halfHeight, Vec3 position, Quat rotation)
{
	Shape shapeResult;

	NvFlexCollisionGeometry geo;
	geo.capsule.radius = radius;
	geo.capsule.halfHeight = halfHeight;

	shapeResult.geometry = geo;

	shapeResult.position = Vec4(position, 0.0f);
	shapeResult.rotation = rotation;

	shapeResult.prevPosition = Vec4(position, 0.0f);
	shapeResult.prevRotation = rotation;

	shapeResult.flag = NvFlexMakeShapeFlags(eNvFlexShapeCapsule, false);

	return shapeResult;
}

Shape ResizeCapsule(Shape shape, float radius, float halfHeight, Vec3 position, Quat rotation) {

	shape.geometry.capsule.halfHeight = halfHeight;
	shape.geometry.capsule.radius = radius;

	// утечка памяти возможно
	shape.position = Vec4(position, 0.0f);
	shape.rotation = rotation;

	shape.prevPosition = Vec4(position, 0.0f);;
	shape.prevRotation = rotation;

	return shape;
}
