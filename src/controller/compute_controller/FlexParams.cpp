#include "FlexParams.h"
#include "../../scenes.h"

void FlexParams::InitFlexParams(Scene *scene) {
	// sim params
	params.gravity[0] = 0.0f;
	params.gravity[1] = -9.8f;
	params.gravity[2] = 0.0f;

	params.radius = 0.15f;
	params.viscosity = 0.0f;
	params.dynamicFriction = 0.0f;
	params.staticFriction = 0.0f;
	params.particleFriction = 0.0f; // scale friction between particles by default
	params.freeSurfaceDrag = 0.0f;
	params.drag = 0.0f;
	params.lift = 0.0f;
	params.numIterations = 3;
	params.fluidRestDistance = 0.0f;
	params.solidRestDistance = 0.0f;

	params.anisotropyScale = 1.0f;
	params.anisotropyMin = 0.1f;
	params.anisotropyMax = 2.0f;
	params.smoothing = 1.0f;

	params.dissipation = 0.0f;
	params.damping = 0.0f;
	params.particleCollisionMargin = 0.0f;
	params.shapeCollisionMargin = 0.0f;
	params.collisionDistance = 0.0f;
	params.plasticThreshold = 0.0f;
	params.plasticCreep = 0.0f;
	params.fluid = false;
	params.sleepThreshold = 0.0f;
	params.shockPropagation = 0.0f;
	params.restitution = 0.0f;

	params.maxSpeed = FLT_MAX;
	params.maxAcceleration = 100.0f;	// approximately 10x gravity

	params.relaxationMode = eNvFlexRelaxationLocal;
	params.relaxationFactor = 1.0f;
	params.solidPressure = 1.0f;
	params.adhesion = 0.0f;
	params.cohesion = 0.025f;
	params.surfaceTension = 0.0f;
	params.vorticityConfinement = 0.0f;
	params.buoyancy = 1.0f;
	params.diffuseThreshold = 100.0f;
	params.diffuseBuoyancy = 1.0f;
	params.diffuseDrag = 0.8f;
	params.diffuseBallistic = 16;
	params.diffuseSortAxis[0] = 0.0f;
	params.diffuseSortAxis[1] = 0.0f;
	params.diffuseSortAxis[2] = 0.0f;
	params.diffuseLifetime = 2.0f;

	// planes created after particles
	params.numPlanes = 1;

	// by default solid particles use the maximum radius
	if (params.fluid && params.solidRestDistance == 0.0f)
		params.solidRestDistance = params.fluidRestDistance;
	else
		params.solidRestDistance = params.radius;

	// collision distance with shapes half the radius
	if (params.collisionDistance == 0.0f)
	{
		params.collisionDistance = params.radius*0.5f;

		if (params.fluid)
			params.collisionDistance = params.fluidRestDistance*0.5f;
	}

	// default particle friction to 10% of shape friction
	if (params.particleFriction == 0.0f)
		params.particleFriction = params.dynamicFriction*0.1f;


	// add a margin for detecting contacts between particles and shapes
	if (params.shapeCollisionMargin == 0.0f)
		params.shapeCollisionMargin = params.collisionDistance*0.5f;

	
	// update collision planes to match flexs
	Vec3 up = Normalize(Vec3(-0.0f, 1.0f, 0.0f));

	Vec3 sceneLower = scene->GetSceneLower();
	Vec3 sceneUpper = scene->GetSceneUpper();

	(Vec4&)params.planes[0] = Vec4(up.x, up.y, up.z, 0.0f);
	(Vec4&)params.planes[1] = Vec4(0.0f, 0.0f, 1.0f, -sceneLower.z);
	(Vec4&)params.planes[2] = Vec4(1.0f, 0.0f, 0.0f, -sceneLower.x);
	(Vec4&)params.planes[3] = Vec4(-1.0f, 0.0f, 0.0f, sceneUpper.x);
	(Vec4&)params.planes[4] = Vec4(0.0f, 0.0f, -1.0f, sceneUpper.z);
	(Vec4&)params.planes[5] = Vec4(0.0f, -1.0f, 0.0f, sceneUpper.y);
}