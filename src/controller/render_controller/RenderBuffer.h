#pragma once

#include <mesh.h>
#include <vector>
#include <map>

#include "Fluid/FluidBuffer.h"

#include "../../shaders.h"

struct RenderBuffers {

	// mesh used for deformable object rendering
	Mesh* mesh = new Mesh();
	
	std::vector<size_t> meshSkinIndices;
	std::vector<float> meshSkinWeights;
	std::vector<Point3> meshRestPositions;
	
	// mapping of collision mesh to render mesh
	std::map<NvFlexConvexMeshId, GpuMesh*> convexes;
	std::map<NvFlexTriangleMeshId, GpuMesh*> meshes;
	std::map<NvFlexDistanceFieldId, GpuMesh*> fields;

	FluidRenderBuffers fluidRenderBuffers;

	// serialize
	template<class Archive>
	void serialize(Archive &archive) {
		archive(*mesh);
		archive(meshSkinIndices, meshSkinWeights, meshRestPositions);
	}
};

