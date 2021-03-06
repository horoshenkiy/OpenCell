#ifndef CAMERA_H
#define CAMERA_H

#include <flex/core/cloth.h>

namespace FruitWork {
namespace Render {

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

	void Initialize(Vec3 sceneLower, Vec3 sceneUpper) {
		camPos = Vec3(
			(sceneLower.x + sceneUpper.x)*0.5f,
			std::min(sceneUpper.y*1.25f, 6.0f),
			sceneUpper.z + std::min(sceneUpper.y, 6.0f)*2.0f
		);

		camAngle = Vec3(0.0f, -DegToRad(15.0f), 0.0f);
	}

	// ������������
	float GetCamSpeed() const {
		return camSpeed;
	}
	void SetCamSpeed(float camSpeed) {
		this->camSpeed = camSpeed;
	}

	Vec3 GetCamVel() const {
		return camVel;
	}
	void SetCamVel(const Vec3 &camVel) {
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

	Vec3 GetCamAngle() const {
		return camAngle;
	}
	void SetCamAngle(const Vec3 &camAngle) {
		this->camAngle = camAngle;
	}

	Vec3 GetCamPos() const {
		return camPos;
	}
	void SetCamPos(const Vec3 &camPos) {
		this->camPos = camPos;
	}

	float GetCamNear() const {
		return this->camNear;
	}
	void SetCamNear(float camNear) {
		this->camNear = camNear;
	}

	float GetCamFar() const {
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

}
}

#endif // CAMERA_H