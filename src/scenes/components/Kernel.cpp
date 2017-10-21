#include "kernel.h"

#include "../../../core/platform.h"

void Kernel::Initialize(SimBuffers *buffers, RenderBuffers *renderBuffers) {
	this->buffers = buffers;
	this->renderBuffers = renderBuffers;

	float radius = 0.05f;
	float restDistance = radius*0.5f;

	int n = 1;
	float spacing = 64 * restDistance*0.9f / (2.0f*n);
	float sampling = restDistance*0.8f;
	Vec3 size = sampling*12.0f;

	const float mass[] = { 1.0f, 0.25f, 0.005f };

	int startBuffers = buffers->positions.size();
	
	CreateParticleShape(buffers, 
						renderBuffers, 
						GetFilePathByPlatform("../../data/sphere.ply").c_str(), 
						Vec3(2.2f, 0.7f, 1.0f), 
						size, 0.0f, sampling, 
						Vec3(0.0f, 0.0f, 0.0f), 
						mass[0],
						true, 
						1.0, 
						NvFlexMakePhase(group++, 0), 
						true, 
						0.0001f);

	int endBuffers = buffers->positions.size();

	Vec3 startVec = buffers->positions[startBuffers];
	float minX = startVec.x, maxX = startVec.x;
	float minY = startVec.y, maxY = startVec.y;
	float minZ = startVec.z, maxZ = startVec.z;

	for (int i = startBuffers; i < endBuffers; i++) {
		minX = (minX > buffers->positions[i].x) ? buffers->positions[i].x : minX;
		maxX = (maxX < buffers->positions[i].x) ? buffers->positions[i].x : maxX;

		minY = (minY > buffers->positions[i].y) ? buffers->positions[i].y : minY;
		maxY = (maxY < buffers->positions[i].y) ? buffers->positions[i].y : maxY;

		minZ = (minZ > buffers->positions[i].z) ? buffers->positions[i].z : minZ;
		maxZ = (maxZ < buffers->positions[i].z) ? buffers->positions[i].z : maxZ;
	}

	float xCenter = (maxX - minX) / 2;
	float yCenter = (maxY - minY) / 2;
	float zCenter = (maxZ - minZ) / 2;

	indexCenter = startBuffers;
	float distMin = maxX - minX;
	for (int i = startBuffers; i < endBuffers; i++) {
		float dist = sqrt(sqr(buffers->positions[i].x - xCenter) +
			sqr(buffers->positions[i].y - yCenter) +
			sqr(buffers->positions[i].z - zCenter));

		if (distMin > dist) {
			distMin = dist;
			indexCenter = i;
		}
	}

	positionCenter = buffers->positions[indexCenter];
	prevPositionCenter = positionCenter;

};

void Kernel::Update() {
	prevPositionCenter = positionCenter;
	positionCenter = buffers->positions[indexCenter];

	rateCenter.x = positionCenter.x - prevPositionCenter.x;
	rateCenter.y = positionCenter.y - prevPositionCenter.y;
	rateCenter.z = positionCenter.z - prevPositionCenter.z;
}