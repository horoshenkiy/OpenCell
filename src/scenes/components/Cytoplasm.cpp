#include "cytoplasm.h"

#include "../../../core/platform.h"

#include "../../controller/compute_controller/FlexParams.h"

Cytoplasm::Cytoplasm() : Component() {
	numberOfParticles = 0;
}

void Cytoplasm::Initialize(FlexParams *flexParams) {
	Mesh* mesh = ImportMesh(GetFilePathByPlatform("../../data/sphere_high.ply").c_str());
	Vec3 lower = Vec3(2.0f + 0 * 2.0f, 0.4f + 0 * 1.2f, 1.0f);

	mesh->Normalize();
	mesh->Transform(TranslationMatrix(Point3(lower)));

	std::vector<Vec3> positions(10000);
	RandInit();
	int n = PoissonSample3D(0.45f, flexParams->params.radius*0.42f, &positions[0], positions.size(), 10000);

	const int vertStart = 0 * mesh->GetNumVertices();
	const int vertEnd = vertStart + mesh->GetNumVertices();

	const int phase = NvFlexMakePhase(group, eNvFlexPhaseSelfCollide | eNvFlexPhaseFluid);

	Vec3 center;
	for (size_t v = vertStart; v < vertEnd; ++v)
		center += Vec3(mesh->m_positions[v]);

	center /= float(vertEnd - vertStart);

	//printf("%d, %d - %f %f %f\n", vertStart, vertEnd, center.x, center.y, center.z);

	for (size_t i = 0; i < n; ++i) {
		buffers.positions.push_back(Vec4(center + positions[i], 1.0f));
		buffers.restPositions.push_back(Vec4());
		buffers.velocities.push_back(0.0f);
		buffers.phases.push_back(phase);
	}

	numberOfParticles = n;

	delete mesh;
};