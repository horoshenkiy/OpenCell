#ifndef SERIALIZE_TYPES_H
#define SERIALIZE_TYPES_H

#include <flex/core/maths.h>
#include <flex/core/mesh.h>
#include <cereal/types/vector.hpp>

#include <flex/NvFlexExt.h>

template<class Archive>
void serialize(Archive &archive, Vec2 &vec2) {
	archive(vec2.x, vec2.y);
}

template<class Archive>
void serialize(Archive &archive, Vec3 &vec3) {
	archive(vec3.x, vec3.y, vec3.z);
}

template<class Archive>
void serialize(Archive &archive, Vec4 &vec4) {
	archive(vec4.x, vec4.y, vec4.z, vec4.w);
}

template<class Archive>
void serialize(Archive &archive, Point3 &point) {
	archive(point.x, point.y, point.z);
}

template<class Archive>
void serialize(Archive &archive, Quat &quat) {
	archive(quat.x, quat.y, quat.z, quat.w);
}

template<class Archive>
void serialize(Archive &archive, Colour &colour) {
	archive(colour.r, colour.g, colour.b, colour.a);
}

///// NvFlexCollisionGeometry
///////////////////////////////////////////////////////
template<class Archive>
void save(Archive &archive, const NvFlexCollisionGeometry &collGeom) {
	archive.saveBinary((char*)&collGeom, sizeof(NvFlexCollisionGeometry));
}

template<class Archive>
void load(Archive &archive, NvFlexCollisionGeometry &collGeom) {
	archive.loadBinary((char*)&collGeom, sizeof(NvFlexCollisionGeometry));
}

///// For NvFlexExtAsset
//////////////////////////////////////////////////////
template<class Archive>
void save(Archive &archive, const NvFlexExtAsset &asset) {
	// particles
	archive(asset.numParticles);
	archive.saveBinary(asset.particles, sizeof(float) * asset.numParticles);
	archive(asset.maxParticles);

	// springs 
	archive(asset.numSprings);
	archive.saveBinary(asset.springIndices, sizeof(int) * asset.numSprings);
	archive.saveBinary(asset.springCoefficients, sizeof(float) * asset.numSprings);
	archive.saveBinary(asset.springRestLengths, sizeof(float) * asset.numSprings);

	// shapes
	archive(asset.numShapeIndices);
	archive.saveBinary(asset.shapeIndices, sizeof(int) * asset.numShapeIndices);

	archive(asset.numShapes);
	archive.saveBinary(asset.shapeOffsets, sizeof(int) * asset.numShapes);
	archive.saveBinary(asset.shapeCoefficients, sizeof(float) * asset.numShapes);
	archive.saveBinary(asset.shapeCenters, sizeof(float) * asset.numShapes);

	// faces for cloth
	archive(asset.numTriangles);
	archive.saveBinary(asset.triangleIndices, sizeof(int) * asset.numTriangles);

	// inflatable params
	archive(asset.inflatable, asset.inflatableVolume, asset.inflatablePressure, asset.inflatableStiffness);
}

template<class Archive>
void load(Archive &archive, NvFlexExtAsset &asset) {
	// particles
	archive(asset.numParticles);
	asset.particles = new float[asset.numParticles];
	archive.loadBinary(asset.particles, sizeof(float) * asset.numParticles);
	archive(asset.maxParticles);

	// springs 
	archive(asset.numSprings);
	asset.springIndices = new int[asset.numSprings];
	asset.springCoefficients = new float[asset.numSprings];
	asset.springRestLengths = new float[asset.numSprings];
	archive.loadBinary(asset.springIndices, sizeof(int) * asset.numSprings);
	archive.loadBinary(asset.springCoefficients, sizeof(float) * asset.numSprings);
	archive.loadBinary(asset.springRestLengths, sizeof(float) * asset.numSprings);

	// shapes*		
	archive(asset.numShapeIndices);
	asset.shapeIndices = new int[asset.numShapeIndices];
	archive.loadBinary(asset.shapeIndices, sizeof(int) * asset.numShapeIndices);

	archive(asset.numShapes);
	asset.shapeOffsets = new int[asset.numShapes];
	asset.shapeCoefficients = new float[asset.numShapes];
	asset.shapeCenters = new float[asset.numShapes];
	archive.loadBinary(asset.shapeOffsets, sizeof(int) * asset.numShapes);
	archive.loadBinary(asset.shapeCoefficients, sizeof(float) * asset.numShapes);
	archive.loadBinary(asset.shapeCenters, sizeof(float) * asset.numShapes);

	// faces for cloth
	archive(asset.numTriangles);
	asset.triangleIndices = new int[asset.numTriangles];
	archive.loadBinary(asset.triangleIndices, sizeof(int) * asset.numTriangles);

	// inflatable params
	archive(asset.inflatable, asset.inflatableVolume, asset.inflatablePressure, asset.inflatableStiffness);
}

///// FruitVector
///////////////////////////////////////////////////////////

template<class Archive, typename T>
void save(Archive &archive, const FruitVector<T> &vec) {
	archive(vec.size());
	for (size_t i = 0; i < vec.size(); i++)
		archive(vec[i]);
}

template<class Archive, typename T>
void load(Archive &archive, FruitVector<T> &vec) {
	size_t sizeVec;
	archive(sizeVec);

	vec.resize(sizeVec);
	for (size_t i = 0; i < vec.size(); i++)
		archive(vec[i]);
}

///// Mesh
///////////////////////////////////////////////////////////

template<class Archive>
void serialize(Archive &archive, Mesh &mesh) {
	archive(mesh.m_positions);
	archive(mesh.m_normals);
	archive(mesh.m_texcoords);
	archive(mesh.m_indices);
	archive(mesh.m_colours);
}

#endif // SERIALIZE_TYPES_H