#include "SimBuffers.h"

//constructors, destructors and initialize
////////////////////////////////////////////////////////////
SimBuffers::SimBuffers(NvFlexLibrary* l)
{
	positions = FruitNvFlexVector<Vec4>(l);
	restPositions = FruitNvFlexVector<Vec4>(l);
	velocities = FruitNvFlexVector<Vec3>(l);
	phases = FruitNvFlexVector<int>(l);
	densities = FruitNvFlexVector<float>(l);
	activeIndices = FruitNvFlexVector<int>(l);

	anisotropy1 = FruitNvFlexVector<Vec4>(l);
	anisotropy2 = FruitNvFlexVector<Vec4>(l);
	anisotropy3 = FruitNvFlexVector<Vec4>(l);

	normals = FruitNvFlexVector<Vec4>(l);

	smoothPositions = FruitNvFlexVector<Vec4>(l);
	diffusePositions = FruitNvFlexVector<Vec4>(l);
	diffuseVelocities = FruitNvFlexVector<Vec4>(l);
	diffuseIndices = FruitNvFlexVector<int>(l);

	shapeGeometry = FruitNvFlexVector<NvFlexCollisionGeometry>(l);
	shapePositions = FruitNvFlexVector<Vec4>(l);
	shapeRotations = FruitNvFlexVector<Quat>(l);
	shapePrevPositions = FruitNvFlexVector<Vec4>(l);
	shapePrevRotations = FruitNvFlexVector<Quat>(l);
	shapeFlags = FruitNvFlexVector<int>(l);

	rigidOffsets = FruitNvFlexVector<int>(l);
	rigidIndices = FruitNvFlexVector<int>(l);
	rigidMeshSize = FruitNvFlexVector<int>(l);
	rigidCoefficients = FruitNvFlexVector<float>(l);
	rigidRotations = FruitNvFlexVector<Quat>(l);
	rigidTranslations = FruitNvFlexVector<Vec3>(l);
	rigidLocalPositions = FruitNvFlexVector<Vec3>(l);
	rigidLocalNormals = FruitNvFlexVector<Vec4>(l);

	inflatableTriOffsets = FruitNvFlexVector<int>(l);
	inflatableTriCounts = FruitNvFlexVector<int>(l);
	inflatableVolumes = FruitNvFlexVector<float>(l);
	inflatableCoefficients = FruitNvFlexVector<float>(l);
	inflatablePressures = FruitNvFlexVector<float>(l);

	springIndices = FruitNvFlexVector<int>(l);
	springLengths = FruitNvFlexVector<float>(l);
	springStiffness = FruitNvFlexVector<float>(l);

	triangles = FruitNvFlexVector<int>(l);
	triangleNormals = FruitNvFlexVector<Vec3>(l);
	uvs = FruitNvFlexVector<Vec3>(l);
}

SimBuffers::~SimBuffers()
{
	// particles
	this->positions.destroy();
	this->restPositions.destroy();
	this->velocities.destroy();
	this->phases.destroy();
	this->densities.destroy();
	this->anisotropy1.destroy();
	this->anisotropy2.destroy();
	this->anisotropy3.destroy();
	this->normals.destroy();
	this->diffusePositions.destroy();
	this->diffuseVelocities.destroy();
	this->diffuseIndices.destroy();
	this->smoothPositions.destroy();
	this->activeIndices.destroy();

	// convexes
	this->shapeGeometry.destroy();
	this->shapePositions.destroy();
	this->shapeRotations.destroy();
	this->shapePrevPositions.destroy();
	this->shapePrevRotations.destroy();
	this->shapeFlags.destroy();

	// rigids
	this->rigidOffsets.destroy();
	this->rigidIndices.destroy();
	this->rigidMeshSize.destroy();
	this->rigidCoefficients.destroy();
	this->rigidRotations.destroy();
	this->rigidTranslations.destroy();
	this->rigidLocalPositions.destroy();
	this->rigidLocalNormals.destroy();

	// springs
	this->springIndices.destroy();
	this->springLengths.destroy();
	this->springStiffness.destroy();

	// inflatables
	this->inflatableTriOffsets.destroy();
	this->inflatableTriCounts.destroy();
	this->inflatableVolumes.destroy();
	this->inflatableCoefficients.destroy();
	this->inflatablePressures.destroy();

	// triangles
	this->triangles.destroy();
	this->triangleNormals.destroy();
	this->uvs.destroy();
}

void SimBuffers::Initialize() {
	this->positions.resize(0);
	this->velocities.resize(0);
	this->phases.resize(0);

	this->rigidOffsets.resize(0);
	this->rigidIndices.resize(0);
	this->rigidMeshSize.resize(0);
	this->rigidRotations.resize(0);
	this->rigidTranslations.resize(0);
	this->rigidCoefficients.resize(0);
	this->rigidLocalPositions.resize(0);
	this->rigidLocalNormals.resize(0);

	this->springIndices.resize(0);
	this->springLengths.resize(0);
	this->springStiffness.resize(0);
	this->triangles.resize(0);
	this->triangleNormals.resize(0);
	this->uvs.resize(0);

	this->shapeGeometry.resize(0);
	this->shapePositions.resize(0);
	this->shapeRotations.resize(0);
	this->shapePrevPositions.resize(0);
	this->shapePrevRotations.resize(0);
	this->shapeFlags.resize(0);


}

void SimBuffers::PostInitialize() {
	this->diffusePositions.resize(maxDiffuseParticles);
	this->diffuseVelocities.resize(maxDiffuseParticles);
	this->diffuseIndices.resize(maxDiffuseParticles);

	// for fluid rendering these are the Laplacian smoothed positions
	this->smoothPositions.resize(maxParticles);

	this->normals.resize(0);
	this->normals.resize(maxParticles);

	// initialize normals (just for rendering before simulation starts)
	size_t numTris = this->triangles.size() / 3;
	for (size_t i = 0; i < numTris; ++i)
	{
		Vec3 v0 = Vec3(this->positions.get(this->triangles[i * 3 + 0]));
		Vec3 v1 = Vec3(this->positions.get(this->triangles[i * 3 + 1]));
		Vec3 v2 = Vec3(this->positions.get(this->triangles[i * 3 + 2]));

		Vec3 n = Cross(v1 - v0, v2 - v0);

		this->normals[this->triangles[i * 3 + 0]] += Vec4(n, 0.0f);
		this->normals[this->triangles[i * 3 + 1]] += Vec4(n, 0.0f);
		this->normals[this->triangles[i * 3 + 2]] += Vec4(n, 0.0f);
	}

	for (size_t i = 0; i < int(maxParticles); ++i)
		this->normals[i] = Vec4(SafeNormalize(Vec3(this->normals[i]), Vec3(0.0f, 1.0f, 0.0f)), 0.0f);

	// create active indices (just a contiguous block for the demo)
	this->activeIndices.resize(this->positions.size());
	for (size_t i = 0; i < this->activeIndices.size(); ++i)
		this->activeIndices[i] = i;

	// save rest positions
	this->restPositions.resize(this->positions.size());
	for (int i = 0; i < this->positions.size(); ++i)
		this->restPositions[i] = this->positions[i];
}

// mapping buffers
///////////////////////////////////////////

void SimBuffers::MapBuffers()
{
	this->positions.map();
	this->restPositions.map();
	this->velocities.map();
	this->phases.map();
	this->densities.map();
	this->anisotropy1.map();
	this->anisotropy2.map();
	this->anisotropy3.map();
	this->normals.map();
	this->diffusePositions.map();
	this->diffuseVelocities.map();
	this->diffuseIndices.map();
	this->smoothPositions.map();
	this->activeIndices.map();

	// convexes
	this->shapeGeometry.map();
	this->shapePositions.map();
	this->shapeRotations.map();
	this->shapePrevPositions.map();
	this->shapePrevRotations.map();
	this->shapeFlags.map();

	this->rigidOffsets.map();
	this->rigidIndices.map();
	this->rigidMeshSize.map();
	this->rigidCoefficients.map();
	this->rigidRotations.map();
	this->rigidTranslations.map();
	this->rigidLocalPositions.map();
	this->rigidLocalNormals.map();

	this->springIndices.map();
	this->springLengths.map();
	this->springStiffness.map();

	// inflatables
	this->inflatableTriOffsets.map();
	this->inflatableTriCounts.map();
	this->inflatableVolumes.map();
	this->inflatableCoefficients.map();
	this->inflatablePressures.map();

	this->triangles.map();
	this->triangleNormals.map();
	this->uvs.map();
}

void SimBuffers::UnmapBuffers()
{
	// particles
	this->positions.unmap();
	this->restPositions.unmap();
	this->velocities.unmap();
	this->phases.unmap();
	this->densities.unmap();
	this->anisotropy1.unmap();
	this->anisotropy2.unmap();
	this->anisotropy3.unmap();
	this->normals.unmap();
	this->diffusePositions.unmap();
	this->diffuseVelocities.unmap();
	this->diffuseIndices.unmap();
	this->smoothPositions.unmap();
	this->activeIndices.unmap();

	// convexes
	this->shapeGeometry.unmap();
	this->shapePositions.unmap();
	this->shapeRotations.unmap();
	this->shapePrevPositions.unmap();
	this->shapePrevRotations.unmap();
	this->shapeFlags.unmap();

	// rigids
	this->rigidOffsets.unmap();
	this->rigidIndices.unmap();
	this->rigidMeshSize.unmap();
	this->rigidCoefficients.unmap();
	this->rigidRotations.unmap();
	this->rigidTranslations.unmap();
	this->rigidLocalPositions.unmap();
	this->rigidLocalNormals.unmap();

	// springs
	this->springIndices.unmap();
	this->springLengths.unmap();
	this->springStiffness.unmap();

	// inflatables
	this->inflatableTriOffsets.unmap();
	this->inflatableTriCounts.unmap();
	this->inflatableVolumes.unmap();
	this->inflatableCoefficients.unmap();
	this->inflatablePressures.unmap();

	// triangles
	this->triangles.unmap();
	this->triangleNormals.unmap();
	this->uvs.unmap();
}

// build constrains and send data
//////////////////////////////////////////////////

void SimBuffers::BuildConstraints() {
	if (rigidOffsets.size()) {
		assert(rigidOffsets.size() > 1);

		const int numRigids = rigidOffsets.size() - 1;

		// calculate local rest space positions
		rigidLocalPositions.resize(rigidOffsets.back());
		CalculateRigidLocalPositions(&positions.get(0), positions.size(), &rigidOffsets[0], &rigidIndices[0], numRigids, &rigidLocalPositions[0]);

		rigidRotations.resize(rigidOffsets.size() - 1, Quat());
		rigidTranslations.resize(rigidOffsets.size() - 1, Vec3());
	}
}

void SimBuffers::SendBuffers(NvFlexSolver *flex) {
	
	////////////////////////////////////////////////////////
	FruitSolver fruitSolver;
	fruitSolver.SetSolver(flex);

	fruit->SetParticles(fruitSolver, positions.GetBuffer(), numParticles);
	fruit->SetRestParticles(fruitSolver, restPositions.GetBuffer(), restPositions.size());
	fruit->SetVelocities(fruitSolver, velocities.GetBuffer(), velocities.size());
	fruit->SetPhases(fruitSolver, phases.GetBuffer(), phases.size());
	fruit->SetNormals(fruitSolver, normals.GetBuffer(), numParticles);
	fruit->SetActiveIndices(fruitSolver, normals.GetBuffer(), numParticles);

	// springs
	if (springIndices.size()) {
		assert((springIndices.size() & 1) == 0);
		assert((springIndices.size() / 2) == springLengths.size());

		fruit->SetSprings(fruitSolver,
			springIndices.GetBuffer(),
			springLengths.GetBuffer(),
			springStiffness.GetBuffer(),
			springLengths.size());
	}

	// rigids
	if (rigidOffsets.size()) {
		fruit->SetRigids(
			fruitSolver,
			rigidOffsets.GetBuffer(),
			rigidIndices.GetBuffer(),
			rigidLocalPositions.GetBuffer(),
			rigidLocalNormals.GetBuffer(),
			rigidCoefficients.GetBuffer(),
			rigidRotations.GetBuffer(),
			rigidTranslations.GetBuffer(),
			rigidOffsets.size() - 1,
			rigidIndices.size());
	}

	// inflatables
	if (inflatableTriOffsets.size()) {
		fruit->SetInflatables(
			fruitSolver,
			inflatableTriOffsets.GetBuffer(),
			inflatableTriCounts.GetBuffer(),
			inflatableVolumes.GetBuffer(),
			inflatablePressures.GetBuffer(),
			inflatableCoefficients.GetBuffer(),
			inflatableTriOffsets.size());
	}

	// dynamic triangles 
	if (triangles.size()) {
		fruit->SetDynamicTriangles(fruitSolver, triangles.GetBuffer(), triangleNormals.GetBuffer(), triangles.size() / 3);
	}

	// collision shapes
	if (shapeFlags.size()) {
		fruit->SetShapes(
			fruitSolver,
			shapeGeometry.GetBuffer(),
			shapePositions.GetBuffer(),
			shapeRotations.GetBuffer(),
			shapePrevPositions.GetBuffer(),
			shapePrevRotations.GetBuffer(),
			shapeFlags.GetBuffer(),
			int(shapeFlags.size()));
	}
}

// methods for clearing
///////////////////////////////////////////////////////////////
void SimBuffers::ClearShapes() {
	this->shapeGeometry.resize(0);
	this->shapePositions.resize(0);
	this->shapeRotations.resize(0);
	this->shapePrevPositions.resize(0);
	this->shapePrevRotations.resize(0);
	this->shapeFlags.resize(0);
}

// method for logging
///////////////////////////////////////////////////////////////
void SimBuffers::StartLogging() {
	printf("Start logging...\n");
}

void SimBuffers::EndLogging() {
	printf("End logging...\n");
}

