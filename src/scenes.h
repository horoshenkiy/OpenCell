#pragma once

struct FlexParams;

#include "controller/render_controller/RenderParam.h"

#include "controller/compute_controller/FlexController.h"
#include "controller/compute_controller/FlexParams.h"
#include "controller/compute_controller/SimBuffers.h"

class Scene {

protected:

	Vec3 sceneLower = Vec3(FLT_MAX);
	Vec3 sceneUpper = Vec3(-FLT_MAX);

	FlexController *flexController = nullptr;
	FlexParams *flexParams = nullptr;
	SimBuffers *buffers = nullptr;

	RenderBuffers *renderBuffers = nullptr;
	RenderParam *renderParam = nullptr;

	const char* mName;

public:

	virtual ~Scene() {}
	
	Vec3 GetSceneLower() const {
		return sceneLower;
	}
	void SetSceneLower(const Vec3 &sceneLower) {
		this->sceneLower = sceneLower;
	}

	Vec3 GetSceneUpper() const {
		return sceneUpper;
	}
	void SetSceneUpper(const Vec3 &sceneUpper) {
		this->sceneUpper = sceneUpper;
	}

	Scene() = default;

	Scene(const char* name) : mName(name) {}
	 
	virtual void Initialize(FlexController *flexController,
							SimBuffers *buffers,
							FlexParams *flexParams,
							RenderBuffers *renderBuffers,
							RenderParam *renderParam) = 0;
	
	// update any buffers (all guaranteed to be mapped here)
	virtual void Update() {}	

	// send any changes to flex (all buffers guaranteed to be unmapped here)
	virtual void Sync() {}
	
	virtual void Draw() {}
	//virtual void KeyDown(int key) {}
	virtual void DoGui() {}
	virtual void CenterCamera() {}

	virtual Matrix44 GetBasis() { return Matrix44::kIdentity; }	

	virtual const char* GetName() { return mName; }

};