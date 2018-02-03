#pragma once

#include <maths.h>
#include <NvFlexImplFruitExt.h>

#include "scenes/components/kernel.h"
#include "scenes/components/cytoplasm.h"
#include "scenes/components/shell.h"
#include <cereal/archives/binary.hpp>

class SceneCell;
class Cell;

class Serializer {
public:

	///// For SimBuffers
	//////////////////////////////////////////////////////
	static void Save(cereal::BinaryOutputArchive &archive, SimBuffers &buffers) {
		// global vars
		archive(buffers.numParticles, buffers.maxParticles);
		archive(buffers.numExtraParticles, buffers.numExtraMultiplier, buffers.numSolidParticles);

		// data of particles
	    Save(archive, buffers.positions);
		Save(archive, buffers.restPositions);
		Save(archive, buffers.velocities);
		Save(archive, buffers.phases);
		Save(archive, buffers.densities);
		Save(archive, buffers.activeIndices);

		Save(archive, buffers.anisotropy1);
		Save(archive, buffers.anisotropy2);
		Save(archive, buffers.anisotropy3);

		Save(archive, buffers.normals);

		Save(archive, buffers.smoothPositions);
		Save(archive, buffers.diffusePositions);
		Save(archive, buffers.diffuseVelocities);
		Save(archive, buffers.diffuseIndices);

		//shape
		Save(archive, buffers.shapeGeometry);
		Save(archive, buffers.shapePositions);
		Save(archive, buffers.shapeRotations);
		Save(archive, buffers.shapePrevPositions);
		Save(archive, buffers.shapePrevRotations);
		Save(archive, buffers.shapeFlags);

		//rigid
		Save(archive, buffers.rigidOffsets);
		Save(archive, buffers.rigidIndices);
		Save(archive, buffers.rigidMeshSize);
		Save(archive, buffers.rigidCoefficients);
		Save(archive, buffers.rigidRotations);
		Save(archive, buffers.rigidTranslations);
		Save(archive, buffers.rigidLocalPositions);
		Save(archive, buffers.rigidLocalNormals);

		// inflatables
		Save(archive, buffers.inflatableTriOffsets);
		Save(archive, buffers.inflatableTriCounts);
		Save(archive, buffers.inflatableVolumes);
		Save(archive, buffers.inflatableCoefficients);
		Save(archive, buffers.inflatablePressures);

		// springs
		Save(archive, buffers.springIndices);
		Save(archive, buffers.springLengths);
		Save(archive, buffers.springStiffness);

		// dynamic triangles
		Save(archive, buffers.triangles);
		Save(archive, buffers.triangleNormals);
		Save(archive, buffers.uvs);
	}

	static void Load(cereal::BinaryInputArchive &archive, SimBuffers &buffers) {
		// global vars
		archive(buffers.numParticles, buffers.maxParticles);
		archive(buffers.numExtraParticles, buffers.numExtraMultiplier, buffers.numSolidParticles);

		// data of particles
		Load(archive, buffers.positions);
		Load(archive, buffers.restPositions);
		Load(archive, buffers.velocities);
		Load(archive, buffers.phases);
		Load(archive, buffers.densities);
		Load(archive, buffers.activeIndices);

		Load(archive, buffers.anisotropy1);
		Load(archive, buffers.anisotropy2);
		Load(archive, buffers.anisotropy3);

		Load(archive, buffers.normals);

		Load(archive, buffers.smoothPositions);
		Load(archive, buffers.diffusePositions);
		Load(archive, buffers.diffuseVelocities);
		Load(archive, buffers.diffuseIndices);

		//shape
		Load(archive, buffers.shapeGeometry);
		Load(archive, buffers.shapePositions);
		Load(archive, buffers.shapeRotations);
		Load(archive, buffers.shapePrevPositions);
		Load(archive, buffers.shapePrevRotations);
		Load(archive, buffers.shapeFlags);

		//rigid
		Load(archive, buffers.rigidOffsets);
		Load(archive, buffers.rigidIndices);
		Load(archive, buffers.rigidMeshSize);
		Load(archive, buffers.rigidCoefficients);
		Load(archive, buffers.rigidRotations);
		Load(archive, buffers.rigidTranslations);
		Load(archive, buffers.rigidLocalPositions);
		Load(archive, buffers.rigidLocalNormals);

		// inflatables
		Load(archive, buffers.inflatableTriOffsets);
		Load(archive, buffers.inflatableTriCounts);
		Load(archive, buffers.inflatableVolumes);
		Load(archive, buffers.inflatableCoefficients);
		Load(archive, buffers.inflatablePressures);

		// springs
		Load(archive, buffers.springIndices);
		Load(archive, buffers.springLengths);
		Load(archive, buffers.springStiffness);

		// dynamic triangles
		Load(archive, buffers.triangles);
		Load(archive, buffers.triangleNormals);
		Load(archive, buffers.uvs);
	}

	///// For SceneCell
	//////////////////////////////////////////////////////
	template<class Archive>
	static void Save(Archive &archive, SceneCell &scene) {
		Save(archive, *scene.cell);
		Save(archive, scene.sceneLower);
		Save(archive, scene.sceneLower);
	}

	template<class Archive>
	static void Load(Archive &archive, SceneCell &scene) {
		if (scene.cell == nullptr)
			scene.cell = new Cell(scene.buffers, scene.renderBuffers);
		Load(archive, *scene.cell);
		
		Load(archive, scene.sceneLower);
		Load(archive, scene.sceneLower);
	}

	///// For Cell
	//////////////////////////////////////////////////////
	template<class Archive>
	static void Save(Archive &archive, Cell &cell) {
		archive(cell.group, cell.mNumFluidParticles);
		Save(archive, *cell.cytoplasm);
		Save(archive, *cell.kernel);
		Save(archive, *cell.shell);
	}

	template<class Archive>
	static void Load(Archive &archive, Cell &cell) {
		archive(cell.group, cell.mNumFluidParticles);

		if (cell.cytoplasm == nullptr)
			cell.cytoplasm = new Cytoplasm(cell.buffers);

		if (cell.kernel == nullptr)
			cell.kernel = new Kernel(cell.buffers, cell.renderBuffers);

		if (cell.shell == nullptr)
			cell.shell = new Shell(cell.buffers);

		Load(archive, *cell.cytoplasm);
		Load(archive, *cell.kernel);
		Load(archive, *cell.shell);
	}

	///// For Shell
	//////////////////////////////////////////////////////
	template<class Archive> 
	static void Save(Archive &archive, Shell &shell) {
		archive(shell.group, shell.indBeginPosition, shell.indEndPosition, shell.splitThreshold);
		Save(archive, *(shell.asset));
	}

	template<class Archive>
	static void Load(Archive &archive, Shell &shell) {
		archive(shell.group, shell.indBeginPosition, shell.indEndPosition, shell.splitThreshold);

		shell.asset = new NvFlexExtAsset();
		Load(archive, *(shell.asset));
	}

	///// For NvFlexExtAsset
	//////////////////////////////////////////////////////
	template<class Archive>
	static void Save(Archive &archive, NvFlexExtAsset &asset) {
		// particles
		archive(asset.numParticles);
		Save(archive, asset.particles, asset.numParticles);
		archive(asset.maxParticles);

		// springs 
		archive(asset.numSprings);
		Save(archive, asset.springIndices, asset.numSprings);
		Save(archive, asset.springCoefficients, asset.numSprings);
		Save(archive, asset.springRestLengths, asset.numSprings);

		// shapes
		archive(asset.numShapeIndices);
		Save(archive, asset.shapeIndices, asset.numShapeIndices);

		archive(asset.numShapes);
		Save(archive, asset.shapeOffsets, asset.numShapes);
		Save(archive, asset.shapeCoefficients, asset.numShapes);
		Save(archive, asset.shapeCenters, asset.numShapes);

		// faces for cloth
		archive(asset.numTriangles);
		Save(archive, asset.triangleIndices, asset.numTriangles);

		// inflatable params
		archive(asset.inflatable, asset.inflatableVolume, asset.inflatablePressure, asset.inflatableStiffness);
	}

	template<class Archive>
	static void Load(Archive &archive, NvFlexExtAsset &asset) {
		// particles
		archive(asset.numParticles);
		Load(archive, &asset.particles, asset.numParticles);
		archive(asset.maxParticles);

		// springs 
		archive(asset.numSprings);
		Load(archive, &asset.springIndices, asset.numSprings);
		Load(archive, &asset.springCoefficients, asset.numSprings);
		Load(archive, &asset.springRestLengths, asset.numSprings);

		// shapes*		
		archive(asset.numShapeIndices);
		Load(archive, &asset.shapeIndices, asset.numShapeIndices);

		archive(asset.numShapes);
		Load(archive, &asset.shapeOffsets, asset.numShapes);
		Load(archive, &asset.shapeCoefficients, asset.numShapes);
		Load(archive, &asset.shapeCenters, asset.numShapes);

		// faces for cloth
		archive(asset.numTriangles);
		Load(archive, &asset.triangleIndices, asset.numTriangles);

		// inflatable params
		archive(asset.inflatable, asset.inflatableVolume, asset.inflatablePressure, asset.inflatableStiffness);
	}

	///// For Cytoplasm
	//////////////////////////////////////////////////////
	template<class Archive>
	static void Save(Archive &archive, Cytoplasm &cytoplasm) {
		archive(cytoplasm.group, cytoplasm.numberOfParticles);
	}

	template<class Archive>
	static void Load(Archive &archive, Cytoplasm &cytoplasm) {
		archive(cytoplasm.group, cytoplasm.numberOfParticles);
	}
	
	///// For Kernel
	//////////////////////////////////////////////////////
	template<class Archive>
	static void Save(Archive &archive, Kernel &kernel) {
		archive(kernel.group, kernel.indexCenter);
		Save(archive, kernel.positionCenter);
		Save(archive, kernel.prevPositionCenter);
		Save(archive, kernel.rateCenter);
	}

	template<class Archive>
	static void Load(Archive &archive, Kernel &kernel) {
		archive(kernel.group, kernel.indexCenter);
		Load(archive, kernel.positionCenter);
		Load(archive, kernel.prevPositionCenter);
		Load(archive, kernel.rateCenter);
	}

	///// For SimBuffers
	//////////////////////////////////////////////////////
	template<class Archive, typename T>
	static void Save(Archive &archive, SimBuffers &buffers) {
		// data of particles
		Save(archive, buffers.positions);
		Save(archive, buffers.restPositions);
		Save(archive, buffers.velocities);
		Save(archive, buffers.phases);
		Save(archive, buffers.densities);
		Save(archive, buffers.activeIndices);

		Save(archive, buffers.anisotropy1);
		Save(archive, buffers.anisotropy2);
		Save(archive, buffers.anisotropy3);

		Save(archive, buffers.normals);

		Save(archive, buffers.smoothPositions);
		Save(archive, buffers.diffusePositions);
		Save(archive, buffers.diffuseVelocities);
		Save(archive, buffers.diffuseIndices);

		//shape
		Save(archive, buffers.shapeGeometry);
		Save(archive, buffers.shapePositions);
		Save(archive, buffers.shapeRotations);
		Save(archive, buffers.shapePrevPositions);
		Save(archive, buffers.shapePrevRotations);
		Save(archive, buffers.shapeFlags);

		// rigid
		Save(archive, buffers.rigidOffsets);
		Save(archive, buffers.rigidIndices);
		Save(archive, buffers.rigidMeshSize);
		Save(archive, buffers.rigidCoefficients);
		Save(archive, buffers.rigidRotations);
		Save(archive, buffers.rigidTranslations);
		Save(archive, buffers.rigidLocalPositions);
		Save(archive, buffers.rigidLocalNormals);

		// inflatables
		Save(archive, buffers.inflatableTriOffsets);
		Save(archive, buffers.inflatableTriCounts);
		Save(archive, buffers.inflatableVolumes);
		Save(archive, buffers.inflatableCoefficients);
		Save(archive, buffers.inflatablePressures);

		// springs
		Save(archive, buffers.springIndices);
		Save(archive, buffers.springLengths);
		Save(archive, buffers.springStiffness);

		// dynamic triangles
		Save(archive, buffers.triangles);
		Save(archive, buffers.triangleNormals);
		Save(archive, buffers.uvs);
	}

	template<class Archive, typename T>
	static void Load(Archive &archive, SimBuffers &buffers) {
		// data of particles
		Load(archive, buffers.positions);
		Load(archive, buffers.restPositions);
		Load(archive, buffers.velocities);
		Load(archive, buffers.phases);
		Load(archive, buffers.densities);
		Load(archive, buffers.activeIndices);

		Load(archive, buffers.anisotropy1);
		Load(archive, buffers.anisotropy2);
		Load(archive, buffers.anisotropy3);

		Load(archive, buffers.normals);

		Load(archive, buffers.smoothPositions);
		Load(archive, buffers.diffusePositions);
		Load(archive, buffers.diffuseVelocities);
		Load(archive, buffers.diffuseIndices);

		//shape
		Load(archive, buffers.shapeGeometry);
		Load(archive, buffers.shapePositions);
		Load(archive, buffers.shapeRotations);
		Load(archive, buffers.shapePrevPositions);
		Load(archive, buffers.shapePrevRotations);
		Load(archive, buffers.shapeFlags);

		// rigid
		Load(archive, buffers.rigidOffsets);
		Load(archive, buffers.rigidIndices);
		Load(archive, buffers.rigidMeshSize);
		Load(archive, buffers.rigidCoefficients);
		Load(archive, buffers.rigidRotations);
		Load(archive, buffers.rigidTranslations);
		Load(archive, buffers.rigidLocalPositions);
		Load(archive, buffers.rigidLocalNormals);

		// inflatables
		Load(archive, buffers.inflatableTriOffsets);
		Load(archive, buffers.inflatableTriCounts);
		Load(archive, buffers.inflatableVolumes);
		Load(archive, buffers.inflatableCoefficients);
		Load(archive, buffers.inflatablePressures);

		// springs
		Load(archive, buffers.springIndices);
		Load(archive, buffers.springLengths);
		Load(archive, buffers.springStiffness);

		// dynamic triangles
		Load(archive, buffers.triangles);
		Load(archive, buffers.triangleNormals);
		Load(archive, buffers.uvs);
	}

	///// For primitive
	///////////////////////////////////////////////////////
	template<class Archive, typename T>
	static void Save(Archive &archive, T &a) {
		archive(a);
	}

	template<class Archive, typename T>
	static void Load(Archive &archive, T &a) {
		archive(a);
	}

	template<class Archive, typename T>
	static void Save(Archive &archive, T* mas, int n) {
		archive.saveBinary(mas, sizeof(T) * n);
	}

	template<class Archive, typename T>
	static void Load(Archive &archive, T** mas, int n) {
		*mas = new T[n];
		archive.loadBinary(*mas, sizeof(T) * n);
	}

	///// Vec4
	///////////////////////////////////////////////////////
	template<class Archive>
	static void Save(Archive &archive, Vec4 &vec) {
		archive(vec.x, vec.y, vec.z, vec.w);
	}

	template<class Archive>
	static void Load(Archive &archive, Vec4 &vec) {
		archive(vec.x, vec.y, vec.z, vec.w);
	}

	///// Vec3
	///////////////////////////////////////////////////////
	template<class Archive>
	static void Save(Archive &archive, Vec3 &vec) {
		archive(vec.x, vec.y, vec.z);
	}

	template<class Archive>
	static void Load(Archive &archive, Vec3 &vec) {
		archive(vec.x, vec.y, vec.z);
	}

	///// NvFlexCollisionGeometry
	///////////////////////////////////////////////////////

	//// TODO: it's slow, need fast
	template<class Archive>
	static void Save(Archive &archive, NvFlexCollisionGeometry &collGeom) {
		char* masByte = (char*)&collGeom;
		for (size_t i = 0; i < sizeof(NvFlexCollisionGeometry); i++)
			archive(masByte[i]);
	}

	template<class Archive>
	static void Load(Archive &archive, NvFlexCollisionGeometry &collGeom) {
		char* masByte = (char*)&collGeom;
		for (size_t i = 0; i < sizeof(NvFlexCollisionGeometry); i++)
			archive(masByte[i]);
	}

	///// Quad
	///////////////////////////////////////////////////////
	template<class Archive>
	static void Save(Archive &archive, Quat &quat) {
		archive(quat.x, quat.y, quat.z, quat.w);
	}

	template<class Archive>
	static void Load(Archive &archive, Quat &quat) {
		archive(quat.x, quat.y, quat.z, quat.w);
	}

	///// FruitNvFlexVector
	///////////////////////////////////////////////////////
	template<class Archive, typename T = float>
	static void Save(Archive &archive, FruitNvFlexVector<T> &vec) {
		archive(vec.size());
		for (size_t i = 0; i < vec.size(); i++)
			Save(archive, vec[i]);
	}

	template<class Archive, typename T = float>
	static void Load(Archive &archive, FruitNvFlexVector<T> &vec) {
		size_t sizeVec;
		archive(sizeVec);

		vec.resize(sizeVec);
		for (size_t i = 0; i < vec.size(); i++)
			Load(archive, vec[i]);
	}

};

#include  "scenes/SceneCell.h"
#include "scenes/cell.h"