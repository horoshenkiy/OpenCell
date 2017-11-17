#pragma once

#include <controller/compute_controller/FlexController.h>
#include <controller/render_controller/RenderParam.h>
#include <controller/render_controller/RenderController.h>
#include <controller/render_controller/RenderBuffer.h>

#include <Serializer.h>
#include <scenes\SceneCell.h>
#include <cereal\archives\binary.hpp>
#include <NvFlexImplFruitExt.h>
#include <gtest\gtest.h>
#include "platform.h"

bool g_extensions = true;

FlexController flexController;

RenderBuffers *renderBuffers;
RenderParam *renderParam;
RenderController renderController;

std::stringstream ss;

bool operator==(const NvFlexExtAsset &lAsset, const NvFlexExtAsset &rAsset);
bool operator==(const Shell& lShell, const Shell& rShell);
bool operator==(const Cytoplasm &lCytoplasm, const Cytoplasm &rCytoplasm);
bool operator==(const Kernel &lKernel, const Kernel &rKernel);
bool operator==(const Cell &lCell, const Cell &rCell);
bool operator==(const SceneCell &lScene, const SceneCell &rScene);

TEST(StateCase, StateSimBuffers) {
	SceneCell *scene = new SceneCell();
	flexController.InitFlex();

	//input
	SimBuffers *oBuffers = new SimBuffers(flexController.GetLib());
	oBuffers->MapBuffers();
	oBuffers->Initialize();

	//output
	SimBuffers *iBuffers = new SimBuffers(flexController.GetLib());
	iBuffers->MapBuffers();
	iBuffers->Initialize();

	FlexParams *flexParams = new FlexParams();
	flexParams->InitFlexParams(scene);

	renderBuffers = new RenderBuffers();
	renderBuffers->meshSkinIndices.resize(0);
	renderBuffers->meshSkinWeights.resize(0);
	renderBuffers->meshRestPositions.resize(0);

	renderParam = new RenderParam();

	scene->Initialize(&flexController, oBuffers, flexParams, renderBuffers, renderParam);

	cereal::BinaryOutputArchive out(ss);
	Serializer::Save(out, *oBuffers);

	cereal::BinaryInputArchive in(ss);
	Serializer::Load(in, *iBuffers);

	//ASSERT_EQ(*oScene, *iScene);

	delete scene;
	delete oBuffers;
	delete iBuffers;
	delete flexParams;
	delete renderBuffers;
	delete renderParam;
}

TEST(StateCase, StateScene) {
	SceneCell *oScene = new SceneCell();

	flexController.InitFlex();

	SimBuffers *buffers = new SimBuffers(flexController.GetLib());
	buffers->MapBuffers();
	buffers->Initialize();

	FlexParams *flexParams = new FlexParams();
	flexParams->InitFlexParams(oScene);

	renderBuffers = new RenderBuffers();
	renderBuffers->meshSkinIndices.resize(0);
	renderBuffers->meshSkinWeights.resize(0);
	renderBuffers->meshRestPositions.resize(0);

	renderParam = new RenderParam();

	FlexController *temp = &flexController;
	oScene->Initialize(temp, buffers, flexParams, renderBuffers, renderParam);

	cereal::BinaryOutputArchive out(ss);
	Serializer::Save(out, *oScene);

	SceneCell *iScene = new SceneCell("cell", temp, buffers, flexParams, renderBuffers, renderParam);

	cereal::BinaryInputArchive in(ss);
	Serializer::Load(in, *iScene);

	ASSERT_EQ(*oScene, *iScene);
	
	delete oScene;
	delete iScene;
	delete buffers;
	delete flexParams;
	delete renderBuffers;
	delete renderParam;
}

TEST(StateCase, StateCell) {
	Scene *scene = new SceneCell();

	flexController.InitFlex();

	SimBuffers *buffers = new SimBuffers(flexController.GetLib());
	buffers->MapBuffers();
	buffers->Initialize();

	FlexParams *flexParams = new FlexParams();
	flexParams->InitFlexParams(scene);

	renderBuffers = new RenderBuffers();
	renderBuffers->meshSkinIndices.resize(0);
	renderBuffers->meshSkinWeights.resize(0);
	renderBuffers->meshRestPositions.resize(0);

	renderParam = new RenderParam();

	Cell &oCell = Cell(), &iCell = Cell(buffers, renderBuffers);
	oCell.Initialize(&flexController, buffers, flexParams, renderBuffers, renderParam);

	cereal::BinaryOutputArchive out(ss);
	Serializer::Save(out, oCell);

	cereal::BinaryInputArchive in(ss);
	Serializer::Load(in, iCell);

	ASSERT_EQ(iCell, oCell);

	delete scene;
	delete buffers;
	delete flexParams;
	delete renderBuffers;
	delete renderParam;
}

TEST(StateCase, StateShell) {
	flexController.InitFlex();

	SimBuffers *buffers = new SimBuffers(flexController.GetLib());
	buffers->MapBuffers();
	buffers->Initialize();

	Shell oShell(0, buffers);
	oShell.Initialize();

	cereal::BinaryOutputArchive out(ss);
	Serializer::Save(out, oShell);

	Shell iShell(buffers);
	cereal::BinaryInputArchive in(ss);
	Serializer::Load(in, iShell);

	ASSERT_EQ(iShell, oShell);
}

TEST(StateCase, StateNvFlexExtAsset) {
	flexController.InitFlex();

	SimBuffers *buffers = new SimBuffers(flexController.GetLib());
	buffers->MapBuffers();
	buffers->Initialize();

	Mesh* mesh = ImportMesh(GetFilePathByPlatform("../../data/sphere_high.ply").c_str());
	Vec3 lower = Vec3(2.0f + 0 * 2.0f, 0.4f + 0 * 1.2f, 1.0f);

	mesh->Normalize();
	mesh->Transform(TranslationMatrix(Point3(lower)));

	const int numParticles = int(mesh->m_positions.size());
	float invMass = 0.25f;

	// add particles to system
	size_t indBeginPosition = buffers->positions.size();
	int phase = NvFlexMakePhase(0, eNvFlexPhaseSelfCollide | eNvFlexPhaseSelfCollideFilter);

	for (size_t i = 0; i < mesh->GetNumVertices(); ++i) {
		const Vec3 p = Vec3(mesh->m_positions[i]);

		buffers->positions.push_back(Vec4(p.x, p.y, p.z, invMass));
		buffers->restPositions.push_back(Vec4(p.x, p.y, p.z, invMass));

		buffers->velocities.push_back(0.0f);
		buffers->phases.push_back(phase);
	}

	// create asset
	NvFlexExtAsset &oCloth = *(NvFlexExtCreateClothFromMesh((float*)&buffers->positions[indBeginPosition],
		numParticles,
		(int*)&mesh->m_indices[0],
		mesh->GetNumFaces(), 0.4f, 0.0f, 0.0f, 0.0f, 0.0f));

	cereal::BinaryOutputArchive oArchive(ss);
	Serializer::Save(oArchive, oCloth);

	NvFlexExtAsset &iCloth = NvFlexExtAsset();
	cereal::BinaryInputArchive iArchive(ss);
	Serializer::Load(iArchive, iCloth);

	ASSERT_EQ(iCloth, oCloth);
}

TEST(StateCase, StateCytoplasm) {
	flexController.InitFlex();

	SimBuffers *buffers = new SimBuffers(flexController.GetLib());
	buffers->MapBuffers();
	buffers->Initialize();

	renderBuffers = new RenderBuffers();
	renderBuffers->meshSkinIndices.resize(0);
	renderBuffers->meshSkinWeights.resize(0);
	renderBuffers->meshRestPositions.resize(0);

	Scene *scene = new SceneCell();

	FlexParams *flexParams = new FlexParams();
	flexParams->InitFlexParams(scene);

	Cytoplasm oCytoplasm = Cytoplasm(0, buffers);
	oCytoplasm.Initialize(flexParams);

	cereal::BinaryOutputArchive oArchive(ss);
	Serializer::Save(oArchive, oCytoplasm);

	Cytoplasm iCytoplasm = Cytoplasm(buffers);
	cereal::BinaryInputArchive iArchive(ss);
	Serializer::Load(iArchive, iCytoplasm);

	ASSERT_EQ(oCytoplasm, iCytoplasm);

	buffers->UnmapBuffers();
	delete buffers;
	delete renderBuffers;
	delete scene;
	delete flexParams;
}

TEST(StateCase, StateKernel) {
	flexController.InitFlex();

	SimBuffers *buffers = new SimBuffers(flexController.GetLib());
	buffers->MapBuffers();
	buffers->Initialize();

	renderBuffers = new RenderBuffers();
	renderBuffers->meshSkinIndices.resize(0);
	renderBuffers->meshSkinWeights.resize(0);
	renderBuffers->meshRestPositions.resize(0);

	Kernel &oKernel = Kernel(0, buffers, renderBuffers);
	oKernel.Initialize();

	cereal::BinaryOutputArchive oarchive(ss);
	Serializer::Save(oarchive, oKernel);

	Kernel &iKernel = Kernel(buffers, renderBuffers);
	cereal::BinaryInputArchive iarchive(ss);
	Serializer::Load(iarchive, iKernel);

	ASSERT_EQ(iKernel, oKernel);

	buffers->UnmapBuffers();
	delete buffers;
	delete renderBuffers;
}

TEST(StateCase, StatePrimitive) {
	int sizeMas = 100;
	float *oMas = new float[sizeMas];

	for (size_t i = 0; i < sizeMas; i++)
		oMas[i] = i % 32;

	cereal::BinaryOutputArchive oArchive(ss);
	Serializer::Save(oArchive, oMas, sizeMas);

	float *iMas = nullptr;
	cereal::BinaryInputArchive iArchive(ss);
	Serializer::Load(iArchive, &iMas, sizeMas);

	for (int i = 0; i < sizeMas; i++)
		ASSERT_EQ(iMas[i], oMas[i]);

	delete[] oMas;
	delete[] iMas;
}

TEST(StateCase, StateVec4) {
	Vec4 &outputVec = Vec4(1.0f, 2.0f, 3.0f, 4.0f);
	cereal::BinaryOutputArchive oarchive(ss);
	Serializer::Save(oarchive, outputVec);

	Vec4 inputVec;
	cereal::BinaryInputArchive iarchive(ss);
	Serializer::Load(iarchive, inputVec);

	ASSERT_EQ(inputVec.x, outputVec.x);
	ASSERT_EQ(inputVec.y, outputVec.y);
	ASSERT_EQ(inputVec.z, outputVec.z);
	ASSERT_EQ(inputVec.w, outputVec.w);
}

TEST(StateCase, StateVec3) {
	Vec3 &outputVec = Vec3(1.0f, 2.0f, 3.0f);
	cereal::BinaryOutputArchive oarchive(ss);
	Serializer::Save(oarchive, outputVec);

	Vec3 inputVec;
	cereal::BinaryInputArchive iarchive(ss);
	Serializer::Load(iarchive, inputVec);

	ASSERT_EQ(inputVec.x, outputVec.x);
	ASSERT_EQ(inputVec.y, outputVec.y);
	ASSERT_EQ(inputVec.z, outputVec.z);
}

TEST(StateCase, StateNvFlexCollisionGeometry) {
	NvFlexCollisionGeometry oCollGeom, iCollGeom;
	oCollGeom.capsule.halfHeight = 2.0f;
	oCollGeom.capsule.radius = 4.0f;

	cereal::BinaryOutputArchive oarchive(ss);
	Serializer::Save(oarchive, oCollGeom);

	cereal::BinaryInputArchive iarchive(ss);
	Serializer::Load(iarchive, iCollGeom);

	ASSERT_EQ(oCollGeom.capsule.halfHeight, iCollGeom.capsule.halfHeight);
	ASSERT_EQ(iCollGeom.capsule.radius, iCollGeom.capsule.radius);
}

TEST(StateCase, StateQuat) {
	Quat oQuat = Quat(1.0f, 2.0f, 3.0f, 4.0f), iQuat;

	cereal::BinaryOutputArchive oarchive(ss);
	Serializer::Save(oarchive, oQuat);

	cereal::BinaryInputArchive iarchive(ss);
	Serializer::Load(iarchive, iQuat);

	ASSERT_EQ(oQuat.x, iQuat.x);
	ASSERT_EQ(oQuat.y, iQuat.y);
	ASSERT_EQ(oQuat.z, iQuat.z);
	ASSERT_EQ(oQuat.w, iQuat.w);
}

TEST(StateCase, StateFruitNvFlexVector) {
	flexController.InitFlex();

	int sizeFruitVec4 = 100;

	auto outFruitVec4 = FruitNvFlexVector<Vec4>(flexController.GetLib());
	outFruitVec4.map();
	outFruitVec4.resize(sizeFruitVec4);
	auto inFruitVec4 = FruitNvFlexVector<Vec4>(flexController.GetLib());
	inFruitVec4.map();
	inFruitVec4.resize(sizeFruitVec4);

	auto outFruitVec3 = FruitNvFlexVector<Vec3>(flexController.GetLib());
	outFruitVec3.map();
	outFruitVec3.resize(sizeFruitVec4);
	auto inFruitVec3 = FruitNvFlexVector<Vec3>(flexController.GetLib());
	inFruitVec3.map();
	inFruitVec3.resize(sizeFruitVec4);

	auto outFruitFloat = FruitNvFlexVector<float>(flexController.GetLib());
	outFruitFloat.map();
	outFruitFloat.resize(sizeFruitVec4);
	auto inFruitFloat = FruitNvFlexVector<float>(flexController.GetLib());
	inFruitFloat.map();
	inFruitFloat.resize(sizeFruitVec4);

	auto outFruitInt = FruitNvFlexVector<int>(flexController.GetLib());
	outFruitInt.map();
	outFruitInt.resize(sizeFruitVec4);
	auto inFruitInt = FruitNvFlexVector<int>(flexController.GetLib());
	inFruitInt.map();
	inFruitInt.resize(sizeFruitVec4);

	for (int i = 0; i < outFruitVec4.size(); i++) {
		outFruitVec4[i] = Vec4(1.0f, 2.0f, 3.0f, 4.0f);
		outFruitVec3[i] = Vec3(1.0f, 2.0f, 3.0f);
		outFruitFloat[i] = 1.0f;
		outFruitInt[i] = 1;
	}

	cereal::BinaryOutputArchive oarchive(ss);
	Serializer::Save(oarchive, outFruitVec4);
	Serializer::Save(oarchive, outFruitVec3);
	Serializer::Save(oarchive, outFruitFloat);
	Serializer::Save(oarchive, outFruitInt);

	cereal::BinaryInputArchive iarchive(ss);
	Serializer::Load(iarchive, inFruitVec4);
	Serializer::Load(iarchive, inFruitVec3);
	Serializer::Load(iarchive, inFruitFloat);
	Serializer::Load(iarchive, inFruitInt);

	for (int i = 0; i < inFruitVec4.size(); i++) {
		ASSERT_EQ(inFruitVec4[i].x, outFruitVec4[i].x);
		ASSERT_EQ(inFruitVec4[i].y, outFruitVec4[i].y);
		ASSERT_EQ(inFruitVec4[i].z, outFruitVec4[i].z);
		ASSERT_EQ(inFruitVec4[i].w, outFruitVec4[i].w);

		ASSERT_EQ(inFruitVec3[i].x, outFruitVec3[i].x);
		ASSERT_EQ(inFruitVec3[i].y, outFruitVec3[i].y);
		ASSERT_EQ(inFruitVec3[i].z, outFruitVec3[i].z);

		ASSERT_EQ(inFruitFloat[i], outFruitFloat[i]);
		ASSERT_EQ(inFruitInt[i], outFruitInt[i]);
	}

	outFruitVec4.unmap();
	inFruitVec4.unmap();

	outFruitVec3.unmap();
	inFruitVec3.unmap();

	outFruitFloat.unmap();
	inFruitFloat.unmap();

	outFruitInt.unmap();
	inFruitInt.unmap();
}

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