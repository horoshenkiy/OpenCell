#include <components/shell.h>

#include <fruit/primitives/primitive_factory.h>

using namespace FruitWork::Primitives;
using namespace FruitWork::Utilits;

void Shell::Initialize() {
	Vec3 lower = { 2.5f, 0.8f, 1.5f };

	FactoryMesh factoryMesh;
	Mesh *mesh = factoryMesh.CreateSphere(0.55, 6);
	mesh->Transform(TranslationMatrix(Point3(lower)));

	FactoryCloth factoryCloth;
	cloth = factoryCloth.CreateCloth(mesh, 0.25f, NvFlexMakePhase(group, eNvFlexPhaseSelfCollide | eNvFlexPhaseSelfCollideFilter));

	buffers.numSolidParticles = buffers.positions.size();
	buffers.numExtraParticles = buffers.positions.size();

	delete mesh;
}

void Shell::Update() {
	cloth->AddFluctuations(-0.01f, 0.01f);
	//cloth->CreateAABBTree();
}