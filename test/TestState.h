#pragma once

#include <controller/compute_controller/FlexController.h>
#include <controller/render_controller/RenderParam.h>
#include <controller/render_controller/RenderController.h>
#include <controller/render_controller/RenderBuffer.h>

#include "SerializerTest.h"
#include <scenes\SceneCell.h>
#include <cereal\archives\binary.hpp>
#include <NvFlexImplFruitExt.h>
#include <gtest\gtest.h>
#include "platform.h"

#include "TestBuffers.h"

#include <FruitExt.h>

bool g_extensions = true;

RenderBuffers *renderBuffers;
RenderParam *renderParam;
RenderController renderController;

SerializerTest serializer;
std::stringstream ss;

bool operator==(const NvFlexExtAsset &lAsset, const NvFlexExtAsset &rAsset);
bool operator!=(const NvFlexExtAsset &lAsset, const NvFlexExtAsset &rAsset);

bool operator==(const Shell& lShell, const Shell& rShell);
bool operator!=(const Shell& lShell, const Shell& rShell);

bool operator==(const Cytoplasm &lCytoplasm, const Cytoplasm &rCytoplasm);
bool operator!=(const Cytoplasm &lCytoplasm, const Cytoplasm &rCytoplasm);

bool operator==(const Kernel &lKernel, const Kernel &rKernel);
bool operator!=(const Kernel &lKernel, const Kernel &rKernel);

bool operator==(const Cell &lCell, const Cell &rCell);
bool operator!=(const Cell &lCell, const Cell &rCell);

bool operator==(const SceneCell &lScene, const SceneCell &rScene);
bool operator!=(const SceneCell &lScene, const SceneCell &rScene);

bool operator==(const SimBuffers &lSimBuffers, const SimBuffers &rSimBuffers);
bool operator!=(const SimBuffers &lSimBuffers, const SimBuffers &rSimBuffers);

bool operator==(const Mesh &lMesh, const Mesh &rMesh);
bool operator!=(const Mesh &lMesh, const Mesh &rMesh);

bool operator==(const RenderBuffers &lRenderBuffers, const RenderBuffers &rRenderBuffers);
bool operator!=(const RenderBuffers &lRenderBuffers, const RenderBuffers &rRenderBuffers);

TEST(StateCase, StateFruitNvFlexVector) {
	flexController.InitFlex();

	int size = 100;

	// Vec4
	//////////////////////////////////////////////////////////////////////////////////////////

	FruitVector<Vec4> &outFruitVec4 = FruitNvFlexVector<Vec4>(flexController.GetLib());
	outFruitVec4.map();
	outFruitVec4.resize(size);
	FruitVector<Vec4> &inFruitVec4 = FruitNvFlexVector<Vec4>(flexController.GetLib());
	inFruitVec4.map();
	inFruitVec4.resize(size);

	// Vec3
	//////////////////////////////////////////////////////////////////////////////////////////

	FruitVector<Vec3> &outFruitVec3 = FruitNvFlexVector<Vec3>(flexController.GetLib());
	outFruitVec3.map();
	outFruitVec3.resize(size);
	FruitVector<Vec3> &inFruitVec3 = FruitNvFlexVector<Vec3>(flexController.GetLib());
	inFruitVec3.map();
	inFruitVec3.resize(size);

	// float
	//////////////////////////////////////////////////////////////////////////////////////////

	FruitVector<float> &outFruitFloat = FruitNvFlexVector<float>(flexController.GetLib());
	outFruitFloat.map();
	outFruitFloat.resize(size);
	FruitVector<float> &inFruitFloat = FruitNvFlexVector<float>(flexController.GetLib());
	inFruitFloat.map();
	inFruitFloat.resize(size);

	// int
	//////////////////////////////////////////////////////////////////////////////////////////

	FruitVector<int> &outFruitInt = FruitNvFlexVector<int>(flexController.GetLib());
	outFruitInt.map();
	outFruitInt.resize(size);
	FruitVector<int> &inFruitInt = FruitNvFlexVector<int>(flexController.GetLib());
	inFruitInt.map();
	inFruitInt.resize(size);

	//////////////////////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < outFruitVec4.size(); i++) {
		outFruitVec4[i] = Vec4(1.0f, 2.0f, 3.0f, 4.0f);
		outFruitVec3[i] = Vec3(1.0f, 2.0f, 3.0f);
		outFruitFloat[i] = 1.0f;
		outFruitInt[i] = 1;
	}

	cereal::BinaryOutputArchive oarchive(ss);
	serializer.Save(oarchive, outFruitVec4);
	serializer.Save(oarchive, outFruitVec3);
	serializer.Save(oarchive, outFruitFloat);
	serializer.Save(oarchive, outFruitInt);

	cereal::BinaryInputArchive iarchive(ss);
	serializer.Load(iarchive, inFruitVec4);
	serializer.Load(iarchive, inFruitVec3);
	serializer.Load(iarchive, inFruitFloat);
	serializer.Load(iarchive, inFruitInt);

	ASSERT_EQ(inFruitVec4, outFruitVec4);
	ASSERT_EQ(inFruitVec3, inFruitVec3);
	ASSERT_EQ(inFruitFloat, outFruitFloat);
	ASSERT_EQ(inFruitInt, outFruitInt);

	outFruitVec4.unmap();
	inFruitVec4.unmap();

	outFruitVec3.unmap();
	inFruitVec3.unmap();

	outFruitFloat.unmap();
	inFruitFloat.unmap();

	outFruitInt.unmap();
	inFruitInt.unmap();
}

TEST(StateCase, StateRenderBuffers) {
	SceneCell *scene = new SceneCell();

	SimBuffers &buffers = SimBuffers::Get();
	buffers.MapBuffers();

	FlexParams *flexParams = new FlexParams();
	flexParams->InitFlexParams(scene);

	RenderBuffers &renderBuffers = RenderBuffers::Get();

	renderParam = new RenderParam();

	scene->Initialize(&flexController, flexParams, renderParam);

	buffers.UnmapBuffers();
	
	// save state
	Mesh mesh = *renderBuffers.mesh;
	std::vector<size_t> meshSkinIndices = renderBuffers.meshSkinIndices;
	std::vector<float> meshSkinWeights = renderBuffers.meshSkinWeights;
	std::vector<Point3> meshRestPositions = renderBuffers.meshRestPositions;

	cereal::BinaryOutputArchive out(ss);
	serializer.Save(out, renderBuffers);

	renderBuffers.Destroy();

	////////////////////////////////

	cereal::BinaryInputArchive in(ss);
	serializer.Load(in, renderBuffers);

	ASSERT_EQ(meshSkinIndices, renderBuffers.meshSkinIndices);
	ASSERT_EQ(meshSkinWeights, renderBuffers.meshSkinWeights);
	ASSERT_EQ(meshRestPositions, renderBuffers.meshRestPositions);
	ASSERT_EQ(mesh, *renderBuffers.mesh);

	buffers.Destroy();
	renderBuffers.Destroy();
}
 
TEST(StateCase, StateSimBuffers) {
	
	SceneCell *scene = new SceneCell();

	SimBuffers &buffers = SimBuffers::Get();
	RenderBuffers &renderBuffers = RenderBuffers::Get();
	
	// mapping
	buffers.MapBuffers();

	FlexParams *flexParams = new FlexParams();
	flexParams->InitFlexParams(scene);

	renderParam = new RenderParam();

	scene->Initialize(&flexController, flexParams, renderParam);

	TestSimBuffers oldBuffers(flexController.GetLib());
	oldBuffers.SaveState(buffers);

	cereal::BinaryOutputArchive out(ss);
	buffers.serialize(out);
	buffers.Destroy();

	cereal::BinaryInputArchive in(ss);
	buffers.serialize(in);

	ASSERT_EQ(buffers, (SimBuffers&)oldBuffers);

	buffers.UnmapBuffers();

	buffers.Destroy();
	renderBuffers.Destroy();

	delete scene;
	delete flexParams;
	delete renderParam;
}

TEST(StateCase, StateScene) {
	SceneCell *oScene = new SceneCell();

	//flexController.InitFlex();

	SimBuffers &buffers = SimBuffers::Get();
	RenderBuffers &renderBuffers = RenderBuffers::Get();

	buffers.MapBuffers();

	FlexParams *flexParams = new FlexParams();
	flexParams->InitFlexParams(oScene);

	renderParam = new RenderParam();

	FlexController *temp = &flexController;
	oScene->Initialize(temp, flexParams, renderParam);

	cereal::BinaryOutputArchive out(ss);
	serializer.Save(out, *oScene);

	SceneCell *iScene = new SceneCell("cell", temp, flexParams, renderParam);

	cereal::BinaryInputArchive in(ss);
	serializer.Load(in, *iScene);

	ASSERT_EQ(*oScene, *iScene);

	buffers.UnmapBuffers();
	
	buffers.Destroy();
	renderBuffers.Destroy();
	
	delete oScene;
	delete iScene;
	delete flexParams;
}

TEST(StateCase, StateCell) {
	Scene *scene = new SceneCell();

	SimBuffers &buffers = SimBuffers::Get();
	RenderBuffers &renderBuffers = RenderBuffers::Get();

	////////////////////////////////////////////////////////////
	// Mapping buffers
	buffers.MapBuffers();

	FlexParams *flexParams = new FlexParams();
	flexParams->InitFlexParams(scene);

	renderParam = new RenderParam();

	Cell oCell, iCell;
	oCell.Initialize(&flexController, flexParams, renderParam);

	cereal::BinaryOutputArchive out(ss);
	serializer.Save(out, oCell);

	cereal::BinaryInputArchive in(ss);
	serializer.Load(in, iCell);

	ASSERT_EQ(iCell, oCell);

	buffers.UnmapBuffers();
	// Unmap buffers 
	////////////////////////////////////////////////////////////

	buffers.Destroy();
	renderBuffers.Destroy();

	delete scene;
	delete flexParams;
	delete renderParam;
}

TEST(StateCase, StateShell) {

	SimBuffers &buffers = SimBuffers::Get();
	buffers.MapBuffers();

	Shell oShell;
	oShell.Initialize();

	cereal::BinaryOutputArchive out(ss);
	serializer.Save(out, oShell);

	Shell iShell;
	cereal::BinaryInputArchive in(ss);
	serializer.Load(in, iShell);

	ASSERT_EQ(iShell, oShell);

	buffers.UnmapBuffers();
	buffers.Destroy();
}

TEST(StateCase, StateNvFlexExtAsset) {

	SimBuffers &buffers = SimBuffers::Get();
	
	////////////////////////////////////////////////////////////
	// Mapping buffers
	buffers.MapBuffers();

	Mesh* mesh = ImportMesh("../../data/sphere_high.ply");
	Vec3 lower = Vec3(2.0f, 0.4f, 1.0f);

	mesh->Normalize();
	mesh->Transform(TranslationMatrix(Point3(lower)));

	const int numParticles = int(mesh->m_positions.size());
	float invMass = 0.25f;

	// add particles to system
	size_t indBeginPosition = buffers.positions.size();
	int phase = NvFlexMakePhase(0, eNvFlexPhaseSelfCollide | eNvFlexPhaseSelfCollideFilter);

	for (size_t i = 0; i < mesh->GetNumVertices(); ++i) {
		const Vec3 p = Vec3(mesh->m_positions[i]);

		buffers.positions.push_back(Vec4(p.x, p.y, p.z, invMass));
		buffers.restPositions.push_back(Vec4(p.x, p.y, p.z, invMass));

		buffers.velocities.push_back(0.0f);
		buffers.phases.push_back(phase);
	}

	// create asset
	NvFlexExtAsset oCloth = *(NvFlexExtCreateClothFromMesh((float*)&buffers.positions[indBeginPosition],
															numParticles,
															(int*)&mesh->m_indices[0],
															mesh->GetNumFaces(), 0.4f, 0.0f, 0.0f, 0.0f, 0.0f));

	cereal::BinaryOutputArchive oArchive(ss);
	serializer.Save(oArchive, oCloth);

	NvFlexExtAsset iCloth = NvFlexExtAsset();
	cereal::BinaryInputArchive iArchive(ss);
	serializer.Load(iArchive, iCloth);

	ASSERT_EQ(iCloth, oCloth);

	buffers.UnmapBuffers();
	buffers.Destroy();
	// Unmap buffers 
	////////////////////////////////////////////////////////////
	
}

TEST(StateCase, StateCytoplasm) {

	SimBuffers &buffers = SimBuffers::Get();
	RenderBuffers &renderBuffers = RenderBuffers::Get();

	////////////////////////////////////////////////////////////
	// Mapping buffers
	buffers.MapBuffers();

	Scene *scene = new SceneCell();

	FlexParams *flexParams = new FlexParams();
	flexParams->InitFlexParams(scene);

	Cytoplasm oCytoplasm = Cytoplasm();
	oCytoplasm.Initialize(flexParams);

	cereal::BinaryOutputArchive oArchive(ss);
	serializer.Save(oArchive, oCytoplasm);

	Cytoplasm iCytoplasm = Cytoplasm();
	cereal::BinaryInputArchive iArchive(ss);
	serializer.Load(iArchive, iCytoplasm);

	ASSERT_EQ(oCytoplasm, iCytoplasm);

	buffers.UnmapBuffers();
	// Unmap buffers 
	////////////////////////////////////////////////////////////

	buffers.Destroy();
	renderBuffers.Destroy();

	delete scene;
	delete flexParams;
}

TEST(StateCase, StateKernel) {

	SimBuffers &buffers = SimBuffers::Get();
	RenderBuffers &renderBuffers = RenderBuffers::Get();

	buffers.MapBuffers();

	Kernel oKernel = Kernel();
	oKernel.Initialize();

	cereal::BinaryOutputArchive oarchive(ss);
	serializer.Save(oarchive, oKernel);

	Kernel iKernel = Kernel();
	cereal::BinaryInputArchive iarchive(ss);
	serializer.Load(iarchive, iKernel);

	ASSERT_EQ(iKernel, oKernel);

	buffers.UnmapBuffers();
	
	buffers.Destroy();
	renderBuffers.Destroy();
}

TEST(StateCase, StatePrimitive) {
	int sizeMas = 100;
	float *oMas = new float[sizeMas];

	for (size_t i = 0; i < sizeMas; i++)
		oMas[i] = i % 32;

	cereal::BinaryOutputArchive oArchive(ss);
	serializer.Save(oArchive, oMas, sizeMas);

	float *iMas = nullptr;
	cereal::BinaryInputArchive iArchive(ss);
	serializer.Load(iArchive, &iMas, sizeMas);

	for (int i = 0; i < sizeMas; i++)
		ASSERT_EQ(iMas[i], oMas[i]);

	delete[] oMas;
	delete[] iMas;
}

TEST(StateCase, StateVec4) {
	Vec4 &outputVec = Vec4(1.0f, 2.0f, 3.0f, 4.0f);
	cereal::BinaryOutputArchive oarchive(ss);
	serializer.Save(oarchive, outputVec);

	Vec4 inputVec;
	cereal::BinaryInputArchive iarchive(ss);
	serializer.Load(iarchive, inputVec);

	ASSERT_EQ(inputVec, outputVec);
}

TEST(StateCase, StateVec3) {
	Vec3 outputVec = Vec3(1.0f, 2.0f, 3.0f);
	cereal::BinaryOutputArchive oarchive(ss);
	serializer.Save(oarchive, outputVec);

	Vec3 inputVec;
	cereal::BinaryInputArchive iarchive(ss);
	serializer.Load(iarchive, inputVec);

	ASSERT_EQ(inputVec, outputVec);
}

TEST(StateCase, StateNvFlexCollisionGeometry) {
	NvFlexCollisionGeometry oCollGeom, iCollGeom;
	oCollGeom.capsule.halfHeight = 2.0f;
	oCollGeom.capsule.radius = 4.0f;

	cereal::BinaryOutputArchive oarchive(ss);
	serializer.Save(oarchive, oCollGeom);

	cereal::BinaryInputArchive iarchive(ss);
	serializer.Load(iarchive, iCollGeom);

	ASSERT_EQ(oCollGeom.capsule.halfHeight, iCollGeom.capsule.halfHeight);
	ASSERT_EQ(iCollGeom.capsule.radius, iCollGeom.capsule.radius);
}

TEST(StateCase, StateQuat) {
	Quat oQuat = Quat(1.0f, 2.0f, 3.0f, 4.0f), iQuat;

	cereal::BinaryOutputArchive oarchive(ss);
	serializer.Save(oarchive, oQuat);

	cereal::BinaryInputArchive iarchive(ss);
	serializer.Load(iarchive, iQuat);

	ASSERT_EQ(oQuat, iQuat);
}



// SimBuffers 
/////////////////////////////////////////////////////////////////

bool operator==(const SimBuffers &lBuf, const SimBuffers &rBuf) {
	if (lBuf.numParticles != rBuf.numParticles || lBuf.maxParticles != rBuf.maxParticles)
		return false;

	if (lBuf.numExtraParticles  != rBuf.numExtraParticles  ||
		lBuf.numExtraMultiplier != rBuf.numExtraMultiplier ||
		lBuf.numSolidParticles  != rBuf.numSolidParticles)
		return false;

	// data of particles
	if (lBuf.positions != rBuf.positions ||
		lBuf.restPositions != rBuf.restPositions ||
		lBuf.velocities != rBuf.velocities ||
		lBuf.phases != rBuf.phases ||
		lBuf.densities != rBuf.densities ||
		lBuf.activeIndices != rBuf.activeIndices)
		return false;

	if (lBuf.anisotropy1 != rBuf.anisotropy1 ||
		lBuf.anisotropy2 != rBuf.anisotropy2 ||
		lBuf.anisotropy3 != rBuf.anisotropy3 ||
		lBuf.normals	 != rBuf.normals)
		return false;

	if (lBuf.smoothPositions != rBuf.smoothPositions ||
		lBuf.diffusePositions != rBuf.diffusePositions ||
		lBuf.diffuseVelocities != rBuf.diffuseVelocities ||
		lBuf.diffuseIndices != rBuf.diffuseIndices)
		return false;

	//shape
	// TODO: add operator == for CollisionGeometry
	if (//lBuf.shapeGeometry != rBuf.shapeGeometry ||
		lBuf.shapePositions != rBuf.shapePositions ||
		lBuf.shapeRotations != rBuf.shapeRotations ||
		lBuf.shapePrevPositions != rBuf.shapePrevPositions ||
		lBuf.shapePrevRotations != rBuf.shapePrevRotations ||
		lBuf.shapeFlags != rBuf.shapeFlags)
		return false;

	//rigid
	if (lBuf.rigidOffsets != rBuf.rigidOffsets ||
		lBuf.rigidIndices != rBuf.rigidIndices ||
		lBuf.rigidMeshSize != rBuf.rigidMeshSize ||
		lBuf.rigidCoefficients != rBuf.rigidCoefficients ||
		lBuf.rigidRotations != rBuf.rigidRotations ||
		lBuf.rigidTranslations != rBuf.rigidTranslations ||
		lBuf.rigidLocalPositions != rBuf.rigidLocalPositions ||
		lBuf.rigidLocalNormals != rBuf.rigidLocalNormals)
		return false;

	// inflatables
	if (lBuf.inflatableTriOffsets != rBuf.inflatableTriOffsets ||
		lBuf.inflatableTriCounts != rBuf.inflatableTriCounts ||
		lBuf.inflatableVolumes != rBuf.inflatableVolumes ||
		lBuf.inflatableCoefficients != rBuf.inflatableCoefficients ||
		lBuf.inflatablePressures != rBuf.inflatablePressures)
		return false;

	// springs
	if (lBuf.springIndices != rBuf.springIndices ||
		lBuf.springLengths != rBuf.springLengths ||
		lBuf.springStiffness != rBuf.springStiffness)
		return false;

	// dynamic triangles
	if (lBuf.triangles != rBuf.triangles ||
		lBuf.triangleNormals != rBuf.triangleNormals ||
		lBuf.uvs != rBuf.uvs)
		return false;

	return true;
}

bool operator!=(const SimBuffers &lBuf, const SimBuffers &rBuf) {
	return !(lBuf == rBuf);
}

// NvFlexExtAsset
/////////////////////////////////////////////////////////////////

bool operator==(const NvFlexExtAsset &lAsset, const NvFlexExtAsset &rAsset) {
	// particles
	if (lAsset.numParticles != rAsset.numParticles || lAsset.maxParticles != rAsset.maxParticles)
		return false;

	for (int i = 0; i < lAsset.numParticles; i++)
		if (lAsset.particles[i] != rAsset.particles[i])
			return false;

	// springs
	if (lAsset.numSprings != rAsset.numSprings)
		return false;

	for (int i = 0; i < lAsset.numSprings; i++)
		if (lAsset.springIndices[i] != rAsset.springIndices[i] ||
			lAsset.springCoefficients[i] != rAsset.springCoefficients[i] ||
			lAsset.springRestLengths[i] != rAsset.springRestLengths[i])
			return false;

	// shapes
	if (lAsset.numShapeIndices != rAsset.numShapeIndices)
		return false;

	for (int i = 0; i < lAsset.numShapeIndices; i++)
		if (lAsset.shapeIndices[i] != rAsset.shapeIndices[i])
			return false;

	if (lAsset.numShapes != rAsset.numShapes)
		return false;

	for (int i = 0; i < lAsset.numShapes; i++)
		if (lAsset.shapeOffsets[i] != rAsset.shapeOffsets[i] ||
			lAsset.shapeCoefficients[i] != rAsset.shapeCoefficients[i] ||
			lAsset.shapeCenters[i] != rAsset.shapeCenters[i])
			return false;

	// faces for cloth
	if (lAsset.numTriangles != rAsset.numTriangles)
		return false;

	for (int i = 0; i < lAsset.numTriangles; i++)
		if (lAsset.triangleIndices[i] != rAsset.triangleIndices[i])
			return false;

	// inflatables
	if (lAsset.inflatable != rAsset.inflatable ||
		lAsset.inflatablePressure != rAsset.inflatablePressure ||
		lAsset.inflatableVolume != rAsset.inflatableVolume ||
		lAsset.inflatableStiffness != rAsset.inflatableStiffness)
		return false;

	return true;
}

bool operator!=(const NvFlexExtAsset &lAsset, const NvFlexExtAsset &rAsset) {
	return !(lAsset == rAsset);
}

// Shell
/////////////////////////////////////////////////////////////////

bool operator==(const Shell& lShell, const Shell& rShell) {
	if (lShell.buffers != rShell.buffers)
		return false;
	else if (lShell.group != rShell.group)
		return false;
	else if (lShell.indBeginPosition != rShell.indBeginPosition)
		return false;
	else if (lShell.indEndPosition != rShell.indEndPosition)
		return false;
	else if (lShell.splitThreshold != rShell.splitThreshold)
		return false;
	else if (!(*lShell.asset == *rShell.asset))
		return false;

	return true;
}

bool operator!=(const Shell& lShell, const Shell& rShell) {
	return !(lShell == rShell);
}

// Cytoplasm
/////////////////////////////////////////////////////////////////

bool operator==(const Cytoplasm &lCytoplasm, const Cytoplasm &rCytoplasm) {
	if (lCytoplasm.buffers != rCytoplasm.buffers)
		return false;
	else if (lCytoplasm.group != rCytoplasm.group)
		return false;
	else if (lCytoplasm.numberOfParticles != rCytoplasm.numberOfParticles)
		return false;
	else
		return true;
}

bool operator!=(const Cytoplasm &lCytoplasm, const Cytoplasm &rCytoplasm) {
	return !(lCytoplasm == rCytoplasm);
}

// Kernel
/////////////////////////////////////////////////////////////////

bool operator==(const Kernel &lKernel, const Kernel &rKernel) {
	if (lKernel.buffers != rKernel.buffers)
		return false;
	else if (lKernel.renderBuffers != rKernel.renderBuffers)
		return false;
	else if (lKernel.group != rKernel.group)
		return false;
	else if (lKernel.indexCenter != rKernel.indexCenter)
		return false;
	else if (lKernel.positionCenter != rKernel.positionCenter)
		return false;
	else if (lKernel.prevPositionCenter != rKernel.prevPositionCenter)
		return false;
	else if (lKernel.rateCenter != rKernel.rateCenter)
		return false;
	else
		return true;
}

bool operator!=(const Kernel &lKernel, const Kernel &rKernel) {
	return !(lKernel == rKernel);
}

// Cell
/////////////////////////////////////////////////////////////////

bool operator==(const Cell &lCell, const Cell &rCell) {
	if (lCell.group != rCell.group)
		return false;
	else if (lCell.mNumFluidParticles != rCell.mNumFluidParticles)
		return false;
	else if (!(*lCell.shell == *rCell.shell))
		return false;
	else if (!(*lCell.cytoplasm == *rCell.cytoplasm))
		return false;
	else if (!(*lCell.kernel == *rCell.kernel))
		return false;

	return true;
}

bool operator!=(const Cell &lCell, const Cell &rCell) {
	return !(lCell == rCell);
}

// SceneCell
/////////////////////////////////////////////////////////////////

bool operator==(const SceneCell &lScene, const SceneCell &rScene) {
	if (!(*lScene.cell == *rScene.cell))
		return false;
	else if (lScene.sceneLower.x != rScene.sceneLower.x || 
			 lScene.sceneLower.y != rScene.sceneLower.y ||
			 lScene.sceneLower.z != rScene.sceneLower.z)
		return false;
	else if (lScene.sceneUpper.x != rScene.sceneUpper.x ||
			 lScene.sceneUpper.y != rScene.sceneUpper.y ||
			 lScene.sceneUpper.z != rScene.sceneUpper.z)
		return false;
	else if (lScene.flexController != rScene.flexController)
		return false;
	else if (lScene.flexParams != rScene.flexParams)
		return false;
	else if (lScene.buffers != rScene.buffers)
		return false;
	else if (lScene.renderBuffers != rScene.renderBuffers)
		return false;
	else if (lScene.renderParam != rScene.renderParam)
		return false;

	return true;
}

bool operator!=(const SceneCell &lScene, const SceneCell &rScene) {
	return !(lScene == rScene);
}

// Mesh
/////////////////////////////////////////////////////////////////

bool operator==(const Mesh &lMesh, const Mesh &rMesh) {
	if (lMesh.m_positions != rMesh.m_positions ||
		lMesh.m_normals != rMesh.m_normals ||
		lMesh.m_texcoords[0] != rMesh.m_texcoords[0] ||
		lMesh.m_texcoords[1] != rMesh.m_texcoords[1] ||
		lMesh.m_indices != rMesh.m_indices ||
		lMesh.m_colours != rMesh.m_colours)
		return false;

	return true;
}

bool operator!=(const Mesh &lMesh, const Mesh &rMesh) {
	return !(lMesh == rMesh);
}

// Render Buffers
/////////////////////////////////////////////////////////////////

bool operator==(const RenderBuffers &lRenderBuffers, const RenderBuffers &rRenderBuffers) {
	if (lRenderBuffers.meshRestPositions != rRenderBuffers.meshRestPositions ||
		lRenderBuffers.meshSkinIndices != rRenderBuffers.meshSkinIndices ||
		lRenderBuffers.meshSkinWeights != rRenderBuffers.meshSkinWeights ||
		*lRenderBuffers.mesh != *rRenderBuffers.mesh)
		return false;

	return true;
}

bool operator!=(const RenderBuffers &lRenderBuffers, const RenderBuffers &rRenderBuffers) {
	return !(lRenderBuffers == rRenderBuffers);
}