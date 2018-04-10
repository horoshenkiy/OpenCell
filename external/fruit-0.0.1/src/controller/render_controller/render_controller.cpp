#include <memory>

#include "fruit/controller/compute_controller/flex_compute_controller.h"
#include <controller/render_controller/render_controller.h>

namespace FruitWork {
namespace Render {

void RenderController::SkinMesh() {
	if (renderBuffers->mesh)
	{
		size_t startVertex = 0;

		for (size_t r = 0; r < buffers->rigidRotations.size(); ++r)
		{
			const Matrix33 rotation = buffers->rigidRotations[r];
			const size_t numVertices = buffers->rigidMeshSize[r];

			for (size_t i = startVertex; i < numVertices + startVertex; ++i)
			{
				Vec3 skinPos;

				for (size_t w = 0; w < 4; ++w) {
					assert(renderBuffers->meshSkinWeights[i * 4 + w] < FLT_MAX);

					size_t index = renderBuffers->meshSkinIndices[i * 4 + w];
					float weight = renderBuffers->meshSkinWeights[i * 4 + w];

					Point3(buffers->restPositions[index]);
					Vec3(buffers->positions[index]);

					skinPos += (rotation*(renderBuffers->meshRestPositions[i] - Point3(buffers->restPositions[index])) + Vec3(buffers->positions[index]))*weight;
				}
				
				renderBuffers->mesh->m_positions[i] = Point3(skinPos);
			}

			startVertex += numVertices;
		}

		renderBuffers->mesh->CalculateNormals();
	}
}

// main render
void RenderController::RenderScene(int numParticles, int numDiffuse)
{
	//---------------------------------------------------
	// use VBO buffer wrappers to allow Flex to write directly to the OpenGL buffers
	// Flex will take care of any CUDA interop mapping/unmapping during the get() operations
	if (numParticles)
	{
		if (flexParams->interop) {
			// copy data directly from solver to the renderer buffers
			Fluid::UpdateFluidRenderBuffers(renderBuffers->fluidRenderBuffers, 
									flexController->GetSolver(), 
									renderParam->drawEllipsoids, 
									renderParam->drawDensity);
		}
		else {
			// copy particle data to GPU render device

			if (renderParam->drawEllipsoids) {
				// if fluid surface rendering then update with smooth positions and anisotropy
				Fluid::UpdateFluidRenderBuffers(
					renderBuffers->fluidRenderBuffers,
					&buffers->smoothPositions[0],
					(renderParam->drawDensity) ? &buffers->densities[0] : (float*)&buffers->phases[0],
					&buffers->anisotropy1[0],
					&buffers->anisotropy2[0],
					&buffers->anisotropy3[0],
					buffers->positions.size(),
					&buffers->activeIndices[0],
					numParticles);
			}
			else
			{
				// otherwise just send regular positions and no anisotropy
				Fluid::UpdateFluidRenderBuffers(
					renderBuffers->fluidRenderBuffers,
					&buffers->positions[0],
					(float*)&buffers->phases[0],
					NULL, NULL, NULL,
					buffers->positions.size(),
					&buffers->activeIndices[0],
					numParticles);
			}
		}
	}

	//---------------------------------------
	// setup view and state

	float fov = kPi / 4.0f;
	float aspect = float(screenWidth) / float(_screenHeight);

	Vec3 camAngle = camera->GetCamAngle();
	Vec3 camPos = camera->GetCamPos();

	Matrix44 proj = ProjectionMatrix(RadToDeg(fov), aspect, camera->GetCamNear(), camera->GetCamFar());
	Matrix44 view = RotationMatrix(-camAngle.x, Vec3(0.0f, 1.0f, 0.0f))*RotationMatrix(-camAngle.y, Vec3(cosf(-camAngle.x), 0.0f, sinf(-camAngle.x)))*TranslationMatrix(-Point3(camPos));

	//------------------------------------
	// lighting pass

	Vec3 sceneLower = scene->GetSceneLower();
	Vec3 sceneUpper = scene->GetSceneUpper();

	// expand scene bounds to fit most 
	scene->SetSceneLower(Min(sceneLower, Vec3(-2.0f, 0.0f, -2.0f)));
	scene->SetSceneUpper(Max(sceneUpper, Vec3(2.0f, 2.0f, 2.0f)));

	Vec3 sceneExtents = sceneUpper - sceneLower;
	Vec3 sceneCenter = 0.5f * (sceneUpper + sceneLower);

	lightDir = Normalize(Vec3(5.0f, 15.0f, 7.5f));
	lightPos = sceneCenter + lightDir * Length(sceneExtents) * lightDistance;
	lightTarget = sceneCenter;

	// calculate tight bounds for shadow frustum
	float lightFov = 2.0f*atanf(Length(sceneUpper - sceneCenter) / Length(lightPos - sceneCenter));

	// scale and clamp fov for aesthetics
	lightFov = Clamp(lightFov, DegToRad(25.0f), DegToRad(65.0f));

	Matrix44 lightPerspective = ProjectionMatrix(RadToDeg(lightFov), 1.0f, 1.0f, 1000.0f);
	Matrix44 lightView = LookAtMatrix(Point3(lightPos), Point3(lightTarget));
	Matrix44 lightTransform = lightPerspective*lightView;

	// non-fluid particles maintain radius distance (not 2.0f*radius) so multiply by a half
	float radius = flexParams->params.solidRestDistance;

	// fluid particles overlap twice as much again, so half the radius again
	if (flexParams->params.fluid)
		radius = flexParams->params.fluidRestDistance;

	radius *= 0.5f;
	radius *= renderParam->pointScale;

	//-------------------------------------
	// shadowing pass 

	if (renderBuffers->meshSkinIndices.size())
		SkinMesh();

	// create shadow maps
	GL::ShadowBegin(shadowMap);

	GL::SetView(lightView, lightPerspective);
	GL::SetCullMode(false);

	// give scene a chance to do custom drawing
	scene->Draw();

	if (renderParam->drawMesh)
		GL::DrawMesh(renderBuffers->mesh, renderParam->meshColor);

	GL::DrawShapes();

	if (renderParam->drawCloth && buffers->triangles.size()) {
		GL::DrawCloth(&buffers->positions[0], 
			&buffers->normals[0], 
			buffers->uvs.size() ? &buffers->uvs[0].x : NULL, 
			&buffers->triangles[0], 
			buffers->triangles.size() / 3, 
			buffers->positions.size(), 3, 
			renderParam->expandCloth);
	}

	size_t shadowParticles = numParticles;
	size_t shadowParticlesOffset = 0;

	if (!renderParam->drawPoints)
	{
		shadowParticles = 0;

		if (renderParam->drawEllipsoids && flexParams->params.fluid)
		{
			shadowParticles = numParticles - buffers->numSolidParticles;
			shadowParticlesOffset = buffers->numSolidParticles;
		}
	}
	else
	{
		size_t offset = renderParam->drawMesh ? buffers->numSolidParticles : 0;

		shadowParticles = numParticles - offset;
		shadowParticlesOffset = offset;
	}

	if (buffers->activeIndices.size())
		GL::DrawPoints(renderBuffers->fluidRenderBuffers.mPositionVBO,
			renderBuffers->fluidRenderBuffers.mDensityVBO,
			renderBuffers->fluidRenderBuffers.mIndices,
			shadowParticles, shadowParticlesOffset,
			radius, 2048, 1.0f, lightFov, lightPos, lightTarget, lightTransform, shadowMap, renderParam->drawDensity);

	GL::ShadowEnd(renderParam->msaaFbo);

	//----------------
	// lighting pass

	GL::BindSolidShader(screenWidth, _screenHeight, lightPos, lightTarget, lightTransform, shadowMap, 0.0f, Vec4(renderParam->clearColor, renderParam->fogDistance));

	GL::SetView(view, proj);
	GL::SetCullMode(true);

	GL::DrawPlanes((Vec4*)flexParams->params.planes, flexParams->params.numPlanes, renderParam->drawPlaneBias);

	if (renderParam->drawMesh)
		GL::DrawMesh(renderBuffers->mesh, renderParam->meshColor);


	GL::DrawShapes();

	if (renderParam->drawCloth && buffers->triangles.size())
		GL::DrawCloth(&buffers->positions[0], 
			      &buffers->normals[0], 
				  buffers->uvs.size() ? &buffers->uvs[0].x : NULL, 
				  &buffers->triangles[0], 
				  buffers->triangles.size() / 3, 
				  buffers->positions.size(), 
				  3, 
				  renderParam->expandCloth);

	// give scene a chance to do custom drawing
	scene->Draw();

	GL::UnbindSolidShader();

	if (renderParam->drawEllipsoids && flexParams->params.fluid)
	{
		// draw solid particles separately
		if (buffers->numSolidParticles && renderParam->drawPoints)
			GL::DrawPoints(renderBuffers->fluidRenderBuffers.mPositionVBO,
				renderBuffers->fluidRenderBuffers.mDensityVBO,
				renderBuffers->fluidRenderBuffers.mIndices,
				buffers->numSolidParticles, 0, radius, float(screenWidth),
				aspect, fov, lightPos, lightTarget, lightTransform, shadowMap, renderParam->drawDensity);


		int numSolidParticles = buffers->numSolidParticles;
		//int numSolidParticles = 0;
		
		GL::RenderEllipsoids(
			fluidRenderer,
			renderBuffers->fluidRenderBuffers,
			renderBuffers->mesh,
			numParticles - numSolidParticles,
			numSolidParticles, radius,
			float(screenWidth),
			aspect, renderParam->msaaFbo, fov, lightPos, lightTarget,
			lightTransform, shadowMap, Vec4(0.88f, 0.84f, 0.33f, 0.7f),
			renderParam->blur, renderParam->ior, renderParam->drawOpaque);
		
		numSolidParticles = 0;

		// render fluid surface
		GL::RenderEllipsoids(
			fluidRenderer,
			renderBuffers->fluidRenderBuffers,
			renderBuffers->mesh,
			numParticles - numSolidParticles,
			numSolidParticles, radius,
			float(screenWidth),
			aspect, renderParam->msaaFbo, fov, lightPos, lightTarget,
			lightTransform, shadowMap, Vec4(0.26f, 0.77f, 0.27f, 1.0f),
			renderParam->blur, renderParam->ior, renderParam->drawOpaque);
	}
	else
	{
		// draw all particles as spheres
		if (renderParam->drawPoints)
		{
			//int offset = renderParam->drawMesh ? buffers->numSolidParticles : 0;

			if (buffers->activeIndices.size())
				GL::DrawPoints(renderBuffers->fluidRenderBuffers.mPositionVBO,
					renderBuffers->fluidRenderBuffers.mDensityVBO,
					renderBuffers->fluidRenderBuffers.mIndices,
					numParticles, 0, radius, float(screenWidth),
					aspect, fov, lightPos, lightTarget, lightTransform, shadowMap, renderParam->drawDensity);
		}
	}

}

void RenderController::RenderDebug() {
	// springs
	if (renderParam->drawSprings)
	{
		Vec4 color;

		if (renderParam->drawSprings == 1)
		{
			// stretch 
			color = Vec4(0.0f, 0.0f, 1.0f, 0.8f);
		}
		if (renderParam->drawSprings == 2)
		{
			// tether
			color = Vec4(0.0f, 1.0f, 0.0f, 0.8f);
		}

		GL::BeginLines();

		for (size_t i = 0; i < buffers->springLengths.size(); ++i)
		{
			if (renderParam->drawSprings == 1 && buffers->springStiffness[i] < 0.0f)
				continue;
			if (renderParam->drawSprings == 2 && buffers->springStiffness[i] > 0.0f)
				continue;

			size_t a = buffers->springIndices[i * 2];
			size_t b = buffers->springIndices[i * 2 + 1];

			GL::DrawLine(Vec3(buffers->positions[a]), Vec3(buffers->positions[b]), color);
		}

		GL::EndLines();
	}

	// visualize contacts against the environment
	if (renderParam->drawContacts)
	{
		const int maxContactsPerParticle = 6;

		NvFlexVector<Vec4> contactPlanes(flexController->GetLib(), buffers->positions.size()*maxContactsPerParticle);
		NvFlexVector<Vec4> contactVelocities(flexController->GetLib(), buffers->positions.size()*maxContactsPerParticle);
		NvFlexVector<int> contactIndices(flexController->GetLib(), buffers->positions.size());
		NvFlexVector<unsigned int> contactCounts(flexController->GetLib(), buffers->positions.size());

		NvFlexGetContacts(flexController->GetSolver(), contactPlanes.buffer, contactVelocities.buffer, contactIndices.buffer, contactCounts.buffer);

		// ensure transfers have finished
		contactPlanes.map();
		contactVelocities.map();
		contactIndices.map();
		contactCounts.map();

		GL::BeginLines();

		for (int i = 0; i < int(buffers->activeIndices.size()); ++i)
		{
			const int contactIndex = contactIndices[buffers->activeIndices[i]];
			const unsigned int count = contactCounts[contactIndex];

			const float scale = 0.1f;

			for (unsigned int c = 0; c < count; ++c)
			{
				Vec4 plane = contactPlanes[contactIndex*maxContactsPerParticle + c];

				GL::DrawLine(Vec3(buffers->positions[buffers->activeIndices[i]]),
					Vec3(buffers->positions[buffers->activeIndices[i]]) + Vec3(plane)*scale,
					Vec4(0.0f, 1.0f, 0.0f, 0.0f));
			}
		}

		GL::EndLines();
	}

	if (renderParam->drawBases)
	{
		for (int i = 0; i < int(buffers->rigidRotations.size()); ++i)
		{
			GL::BeginLines();

			float size = 0.1f;

			for (int b = 0; b < 3; ++b)
			{
				Vec3 color;
				color[b] = 1.0f;

				Matrix33 frame(buffers->rigidRotations[i]);

				GL::DrawLine(Vec3(buffers->rigidTranslations[i]),
					Vec3(buffers->rigidTranslations[i] + frame.cols[b] * size),
					Vec4(color, 0.0f));
			}

			GL::EndLines();
		}
	}

	if (renderParam->drawNormals) {
		Application::computeController.GetNormals();

		GL::BeginLines();

		for (int i = 0; i < buffers->normals.size(); ++i)
		{
			GL::DrawLine(Vec3(buffers->positions[i]),
				Vec3(buffers->positions[i] - buffers->normals[i] * buffers->normals[i].w),
				Vec4(0.0f, 1.0f, 0.0f, 0.0f));
		}

		GL::EndLines();
	}
}

}
}