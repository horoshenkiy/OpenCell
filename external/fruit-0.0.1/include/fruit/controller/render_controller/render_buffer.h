#ifndef RENDER_BUFFER_H
#define RENDER_BUFFER_H

#include "fluid/fluid_buffer.h"
#include "render_gl/shaders.h"

#include <flex/core/mesh.h>
#include <vector>
#include <map>

namespace FruitWork {
namespace Render {

class RenderBuffers {

public:

	static RenderBuffers& Instance() {
		static RenderBuffers instance;
		return instance;
	}

	static RenderBuffers& Get() {
		return Instance();
	}

	void Initialize(size_t maxParticles, bool interop) {
		// save mesh positions for skinning
		if (mesh)
			meshRestPositions = mesh->m_positions;
		else
			meshRestPositions.resize(0);

		fluidRenderBuffers = Fluid::CreateFluidRenderBuffers(maxParticles, interop);
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
	std::map<NvFlexConvexMeshId, GL::GpuMesh*> convexes;
	std::map<NvFlexTriangleMeshId, GL::GpuMesh*> meshes;
	std::map<NvFlexDistanceFieldId, GL::GpuMesh*> fields;

	Fluid::FluidRenderBuffers fluidRenderBuffers;

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

}
}

#endif // RENDER_BUFFER_H