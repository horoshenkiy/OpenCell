#pragma once

#include "../../../core/cloth.h"
#include "../../../external/SDL2-2.0.4/include/SDL.h"

//#include "scenes.h"

class Camera {

private:
	Vec3 camVel = Vec3(0.0f);
	float camSpeed = 0.075f;

	Vec3 camAngle = Vec3(0.0f, -DegToRad(20.0f), 0.0f);
	Vec3 camPos = Vec3(6.0f, 8.0f, 18.0f);
	Vec3 camSmoothVel = Vec3(0.0F);

	float camNear = 0.01f;
	float camFar = 1000.0f;

public:
	Camera() {}

	// ������������
	float GetCamSpeed() {
		return camSpeed;
	}
	void SetCamSpeed(float camSpeed) {
		this->camSpeed = camSpeed;
	}

	Vec3 GetCamVel() {
		return camVel;
	}
	void SetCamVel(Vec3 camVel) {
		this->camVel = camVel;
	}
	
	void SetCamVelX(float x) {
		this->camVel.x = x;
	}
	void SetCamVelY(float y) {
		this->camVel.y = y;
	}
	void SetCamVelZ(float z) {
		this->camVel.z = z;
	}

	Vec3 GetCamAngle() {
		return camAngle;
	}
	void SetCamAngle(Vec3 camAngle) {
		this->camAngle = camAngle;
	}

	Vec3 GetCamPos() {
		return camPos;
	}
	void SetCamPos(Vec3 camPos) {
		this->camPos = camPos;
	}

	float GetCamNear() {
		return this->camNear;
	}
	void SetCamNear(float camNear) {
		this->camNear = camNear;
	}

	float GetCamFar() {
		return this->camFar;
	}
	void SetCamFar(float camFar) {
		this->camFar = camFar;
	}

	// ���������� ������
	void UpdateCamera()
	{
		Vec3 forward(-sinf(camAngle.x)*cosf(camAngle.y), sinf(camAngle.y), -cosf(camAngle.x)*cosf(camAngle.y));
		Vec3 right(Normalize(Cross(forward, Vec3(0.0f, 1.0f, 0.0f))));

		camSmoothVel = Lerp(camSmoothVel, camVel, 0.1f);
		camPos += (forward * camSmoothVel.z + right * camSmoothVel.x + Cross(right, forward) * camSmoothVel.y);
	}
}; 