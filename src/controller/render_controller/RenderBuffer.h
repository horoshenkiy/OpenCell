#pragma once

#include "core/mesh.h"

#include <vector>
#include <map>

#include "Fluid/FluidBuffer.h"

#include "../../shaders.h"

class RenderBuffers {

public:

	static RenderBuffers& Instance() {
		static RenderBuffers instance;
		return instance;
	}

	static RenderBuffers& Get() {
		return Instance();
	}

	void Destroy() {
		meshSkinIndices.clear();
		meshSkinWeights.clear();
		meshRestPositions.clear();
	}

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

private:

	RenderBuffers() = default;

	RenderBuffers(const RenderBuffers &other) = delete;
	RenderBuffers operator=(const RenderBuffers &other) = delete;
};

