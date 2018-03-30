#ifndef SCENE_H
#define SCENE_H

#include <maths.h>
#include <cfloat>

class Scene {
public:

	// constructors and initializers
	Scene() = default;
	Scene(const char* name) : mName(name) {}
	 
	virtual void Initialize() = 0;

	virtual void InitializeFromFile() = 0;

	virtual void PostInitialize() = 0;

	// destructors
	virtual void Reset() = 0;

	virtual ~Scene() {}
	
	// update any buffers (all guaranteed to be mapped here)
	virtual void Update() {}	

	// send any changes to flex (all buffers guaranteed to be unmapped here)
	virtual void Sync() {}
	
	// for render
	virtual void Draw() {}
	virtual void DoGui() {}
	virtual void CenterCamera() {}

	// getters and setters
	virtual Matrix44 GetBasis() { return Matrix44::kIdentity; }	
	
	virtual const char* GetName() { return mName; }

	Vec3 GetSceneLower() const { return sceneLower; }
	void SetSceneLower(const Vec3 &sceneLower) { this->sceneLower = sceneLower; }

	Vec3 GetSceneUpper() const { return sceneUpper; }
	void SetSceneUpper(const Vec3 &sceneUpper) { this->sceneUpper = sceneUpper; }

protected:

	Vec3 sceneLower = FLT_MAX;
	Vec3 sceneUpper = -FLT_MAX;

	const char* mName;
};

#endif // SCENE_H