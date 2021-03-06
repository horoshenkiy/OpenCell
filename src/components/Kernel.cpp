#include "kernel.h"

#include <flex/core/platform.h>
#include "fruit/utilits/utilits.h"

using namespace FruitWork::Utilits;

void Kernel::Initialize() {

	float radius = 0.05f;
	float restDistance = radius*0.5f;

	int n = 1;
	float spacing = 64 * restDistance*0.9f / (2.0f*n);
	float sampling = restDistance*0.8f;
	Vec3 size = sampling*12.0f;

	const float mass[] = { 1.0f, 0.25f, 0.005f };

	size_t startBuffers = buffers.positions.size();
	
	CreateParticleShape(&buffers, 
						&renderBuffers, 
						"../../data/sphere.ply", 
						Vec3(2.2f, 0.7f, 1.0f), 
						size, 0.0f, sampling, 
						Vec3(0.0f, 0.0f, 0.0f), 
						mass[0],
						true, 
						1.0, 
						NvFlexMakePhase(group, 0), 
						true, 
						0.0001f);

	size_t endBuffers = buffers.positions.size();

	Vec3 startVec = buffers.positions[startBuffers];
	float minX = startVec.x, maxX = startVec.x;
	float minY = startVec.y, maxY = startVec.y;
	float minZ = startVec.z, maxZ = startVec.z;

	for (size_t i = startBuffers; i < endBuffers; i++) {
		minX = (minX > buffers.positions[i].x) ? buffers.positions[i].x : minX;
		maxX = (maxX < buffers.positions[i].x) ? buffers.positions[i].x : maxX;

		minY = (minY > buffers.positions[i].y) ? buffers.positions[i].y : minY;
		maxY = (maxY < buffers.positions[i].y) ? buffers.positions[i].y : maxY;

		minZ = (minZ > buffers.positions[i].z) ? buffers.positions[i].z : minZ;
		maxZ = (maxZ < buffers.positions[i].z) ? buffers.positions[i].z : maxZ;
	}

	float xCenter = minX + (maxX - minX) / 2;
	float yCenter = minY + (maxY - minY) / 2;
	float zCenter = minZ + (maxZ - minZ) / 2;

	trueRadius = (maxX - minX) / 2;

	indexCenter = startBuffers;
	float distMin = maxX - minX;
	for (size_t i = startBuffers; i < endBuffers; i++) {
		float dist = sqrt(sqr(buffers.positions[i].x - xCenter) +
			sqr(buffers.positions[i].y - yCenter) +
			sqr(buffers.positions[i].z - zCenter));

		if (distMin > dist) {
			distMin = dist;
			indexCenter = i;
		}
	}

	positionCenter = buffers.positions[indexCenter];
	prevPositionCenter = positionCenter;

};

void Kernel::Update() {
	prevPositionCenter = positionCenter;
	positionCenter = buffers.positions[indexCenter];

	rateCenter.x = positionCenter.x - prevPositionCenter.x;
	rateCenter.y = positionCenter.y - prevPositionCenter.y;
	rateCenter.z = positionCenter.z - prevPositionCenter.z;
}