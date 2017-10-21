#include "RenderController.h"

void RenderController::SkinMesh()
{
	if (renderBuffers->mesh)
	{
		int startVertex = 0;

		for (int r = 0; r < buffers->rigidRotations.size(); ++r)
		{
			const Matrix33 rotation = buffers->rigidRotations[r];
			const int numVertices = buffers->rigidMeshSize[r];

			for (int i = startVertex; i < numVertices + startVertex; ++i)
			{
				Vec3 skinPos;

				for (int w = 0; w < 4; ++w)
				{
					// small shapes can have < 4 particles
					if (renderBuffers->meshSkinIndices[i * 4 + w] > -1)
					{
						assert(renderBuffers->meshSkinWeights[i * 4 + w] < FLT_MAX);

						int index = renderBuffers->meshSkinIndices[i * 4 + w];
						float weight = renderBuffers->meshSkinWeights[i * 4 + w];

						Point3(buffers->restPositions[index]);
						//renderBuffers->meshRestPositions[i] = (renderBuffers->meshRestPositions[i].x > 2.0) ? 0.0 : renderBuffers->meshRestPositions[i];
						Vec3(buffers->positions[index]);

						skinPos += (rotation*(renderBuffers->meshRestPositions[i] - Point3(buffers->restPositions[index])) + Vec3(buffers->positions[index]))*weight;
					}
				}
				
				renderBuffers->mesh->m_positions[i] = Point3(skinPos);
			}

			startVertex += numVertices;
		}

		renderBuffers->mesh->CalculateNormals();
	}
}

void RenderController::DrawShapes()
{
	for (int i = 0; i < buffers->shapeFlags.size(); ++i)
	{
		const int flags = buffers->shapeFlags[i];

		// unpack flags
		int type = int(flags&eNvFlexShapeFlagTypeMask);
		//bool dynamic = int(flags&eNvFlexShapeFlagDynamic) > 0;

		Vec3 color = Vec3(0.9f);

		if (flags & eNvFlexShapeFlagTrigger)
		{
			color = Vec3(0.6f, 1.0, 0.6f);

			SetFillMode(true);
		}

		// render with prev positions to match particle update order
		// can also think of this as current/next
		const Quat rotation = buffers->shapePrevRotations[i];
		const Vec3 position = Vec3(buffers->shapePrevPositions[i]);

		NvFlexCollisionGeometry geo = buffers->shapeGeometry[i];

		if (type == eNvFlexShapeSphere)
		{
			Mesh* sphere = CreateSphere(20, 20, geo.sphere.radius);

			Matrix44 xform = TranslationMatrix(Point3(position))*RotationMatrix(Quat(rotation));
			sphere->Transform(xform);

			DrawMesh(sphere, Vec3(color));

			delete sphere;
		}
		else if (type == eNvFlexShapeCapsule)
		{
			Mesh* capsule = CreateCapsule(10, 20, geo.capsule.radius, geo.capsule.halfHeight);

			// transform to world space
			Matrix44 xform = TranslationMatrix(Point3(position))*RotationMatrix(Quat(rotation))*RotationMatrix(DegToRad(-90.0f), Vec3(0.0f, 0.0f, 1.0f));
			capsule->Transform(xform);

			DrawMesh(capsule, Vec3(color));

			delete capsule;
		}
		else if (type == eNvFlexShapeBox)
		{
			Mesh* box = CreateCubeMesh();

			Matrix44 xform = TranslationMatrix(Point3(position))*RotationMatrix(Quat(rotation))*ScaleMatrix(Vec3(geo.box.halfExtents)*2.0f);
			box->Transform(xform);

			DrawMesh(box, Vec3(color));
			delete box;
		}
		else if (type == eNvFlexShapeConvexMesh)
		{
			if (renderBuffers->convexes.find(geo.convexMesh.mesh) != renderBuffers->convexes.end())
			{
				GpuMesh* m = renderBuffers->convexes[geo.convexMesh.mesh];

				if (m)
				{
					Matrix44 xform = TranslationMatrix(Point3(buffers->shapePositions[i]))*RotationMatrix(Quat(buffers->shapeRotations[i]))*ScaleMatrix(geo.convexMesh.scale);
					DrawGpuMesh(m, xform, Vec3(color));
				}
			}
		}
		else if (type == eNvFlexShapeTriangleMesh)
		{
			if (renderBuffers->meshes.find(geo.triMesh.mesh) != renderBuffers->meshes.end())
			{
				GpuMesh* m = renderBuffers->meshes[geo.triMesh.mesh];

				if (m)
				{
					Matrix44 xform = TranslationMatrix(Point3(position))*RotationMatrix(Quat(rotation))*ScaleMatrix(geo.triMesh.scale);
					DrawGpuMesh(m, xform, Vec3(color));
				}
			}
		}
		else if (type == eNvFlexShapeSDF)
		{
			if (renderBuffers->fields.find(geo.sdf.field) != renderBuffers->fields.end())
			{
				GpuMesh* m = renderBuffers->fields[geo.sdf.field];

				if (m)
				{
					Matrix44 xform = TranslationMatrix(Point3(position))*RotationMatrix(Quat(rotation))*ScaleMatrix(geo.sdf.scale);
					DrawGpuMesh(m, xform, Vec3(color));
				}
			}
		}
	}

	SetFillMode(renderParam->wireframe);
}

// main render
void RenderController::RenderScene(int numParticles, int numDiffuse)
{
	//---------------------------------------------------
	// use VBO buffer wrappers to allow Flex to write directly to the OpenGL buffers
	// Flex will take care of any CUDA interop mapping/unmapping during the get() operations

	if (numParticles)
	{
		if (flexParams->interop)
		{
			// copy data directly from solver to the renderer buffers
			UpdateFluidRenderBuffers(renderBuffers->fluidRenderBuffers, 
									flexController->GetSolver(), 
									renderParam->drawEllipsoids, 
									renderParam->drawDensity);
		}
		else
		{
			// copy particle data to GPU render device

			if (renderParam->drawEllipsoids)
			{
				// if fluid surface rendering then update with smooth positions and anisotropy
				UpdateFluidRenderBuffers(renderBuffers->fluidRenderBuffers,
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
				UpdateFluidRenderBuffers(renderBuffers->fluidRenderBuffers,
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
	float aspect = float(screenWidth) / float(screenHeight);

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
	ShadowBegin(shadowMap);

	SetView(lightView, lightPerspective);
	SetCullMode(false);

	// give scene a chance to do custom drawing
	scene->Draw(1);

	if (renderParam->drawMesh)
		DrawMesh(renderBuffers->mesh, renderParam->meshColor);

	DrawShapes();

	if (renderParam->drawCloth && buffers->triangles.size()) {
		DrawCloth(&buffers->positions[0], 
			&buffers->normals[0], 
			buffers->uvs.size() ? &buffers->uvs[0].x : NULL, 
			&buffers->triangles[0], 
			buffers->triangles.size() / 3, 
			buffers->positions.size(), 3, 
			renderParam->expandCloth);
	}

	int shadowParticles = numParticles;
	int shadowParticlesOffset = 0;

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
		int offset = renderParam->drawMesh ? buffers->numSolidParticles : 0;

		shadowParticles = numParticles - offset;
		shadowParticlesOffset = offset;
	}

	if (buffers->activeIndices.size())
		DrawPoints(renderBuffers->fluidRenderBuffers.mPositionVBO,
			renderBuffers->fluidRenderBuffers.mDensityVBO,
			renderBuffers->fluidRenderBuffers.mIndices,
			shadowParticles, shadowParticlesOffset,
			radius, 2048, 1.0f, lightFov, lightPos, lightTarget, lightTransform, shadowMap, renderParam->drawDensity);

	ShadowEnd();

	//----------------
	// lighting pass

	BindSolidShader(lightPos, lightTarget, lightTransform, shadowMap, 0.0f, Vec4(renderParam->clearColor, renderParam->fogDistance));

	SetView(view, proj);
	SetCullMode(true);

	DrawPlanes((Vec4*)flexParams->params.planes, flexParams->params.numPlanes, renderParam->drawPlaneBias);

	if (renderParam->drawMesh)
		DrawMesh(renderBuffers->mesh, renderParam->meshColor);


	DrawShapes();

	if (renderParam->drawCloth && buffers->triangles.size())
		DrawCloth(&buffers->positions[0], 
			      &buffers->normals[0], 
				  buffers->uvs.size() ? &buffers->uvs[0].x : NULL, 
				  &buffers->triangles[0], 
				  buffers->triangles.size() / 3, 
				  buffers->positions.size(), 
				  3, 
				  renderParam->expandCloth);

	// give scene a chance to do custom drawing
	scene->Draw(0);

	UnbindSolidShader();

	if (renderParam->drawEllipsoids && flexParams->params.fluid)
	{
		// draw solid particles separately
		if (buffers->numSolidParticles && renderParam->drawPoints)
			DrawPoints(renderBuffers->fluidRenderBuffers.mPositionVBO,
				renderBuffers->fluidRenderBuffers.mDensityVBO,
				renderBuffers->fluidRenderBuffers.mIndices,
				buffers->numSolidParticles, 0, radius, float(screenWidth),
				aspect, fov, lightPos, lightTarget, lightTransform, shadowMap, renderParam->drawDensity);

		// render fluid surface
		RenderEllipsoids(fluidRenderer,
			renderBuffers->fluidRenderBuffers,
			numParticles - buffers->numSolidParticles,
			buffers->numSolidParticles, radius,
			float(screenWidth),
			aspect, fov, lightPos, lightTarget,
			lightTransform, shadowMap, renderParam->fluidColor,
			renderParam->blur, renderParam->ior, renderParam->drawOpaque);
	}
	else
	{
		// draw all particles as spheres
		if (renderParam->drawPoints)
		{
			int offset = renderParam->drawMesh ? buffers->numSolidParticles : 0;
				 
			if (buffers->activeIndices.size())
				DrawPoints(renderBuffers->fluidRenderBuffers.mPositionVBO,
					renderBuffers->fluidRenderBuffers.mDensityVBO,
					renderBuffers->fluidRenderBuffers.mIndices,
					numParticles - offset, offset, radius, float(screenWidth),
					aspect, fov, lightPos, lightTarget, lightTransform, shadowMap, renderParam->drawDensity);
		}
	}

}

void RenderController::RenderDebug()
{
	Fruit *fruit = new FruitNvFlex();
	FruitSolver fruitSolver;
	fruitSolver.SetSolver(flexController->GetSolver());

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

		BeginLines();

		int start = 0;

		for (int i = start; i < buffers->springLengths.size(); ++i)
		{
			if (renderParam->drawSprings == 1 && buffers->springStiffness[i] < 0.0f)
				continue;
			if (renderParam->drawSprings == 2 && buffers->springStiffness[i] > 0.0f)
				continue;

			int a = buffers->springIndices[i * 2];
			int b = buffers->springIndices[i * 2 + 1];

			DrawLine(Vec3(buffers->positions[a]), Vec3(buffers->positions[b]), color);
		}

		EndLines();
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

		BeginLines();

		for (int i = 0; i < int(buffers->activeIndices.size()); ++i)
		{
			const int contactIndex = contactIndices[buffers->activeIndices[i]];
			const unsigned int count = contactCounts[contactIndex];

			const float scale = 0.1f;

			for (unsigned int c = 0; c < count; ++c)
			{
				Vec4 plane = contactPlanes[contactIndex*maxContactsPerParticle + c];

				DrawLine(Vec3(buffers->positions[buffers->activeIndices[i]]),
					Vec3(buffers->positions[buffers->activeIndices[i]]) + Vec3(plane)*scale,
					Vec4(0.0f, 1.0f, 0.0f, 0.0f));
			}
		}

		EndLines();
	}

	if (renderParam->drawBases)
	{
		for (int i = 0; i < int(buffers->rigidRotations.size()); ++i)
		{
			BeginLines();

			float size = 0.1f;

			for (int b = 0; b < 3; ++b)
			{
				Vec3 color;
				color[b] = 1.0f;

				Matrix33 frame(buffers->rigidRotations[i]);

				DrawLine(Vec3(buffers->rigidTranslations[i]),
					Vec3(buffers->rigidTranslations[i] + frame.cols[b] * size),
					Vec4(color, 0.0f));
			}

			EndLines();
		}
	}

	if (renderParam->drawNormals) {
		// rewrite сильная связность модулей
		fruit->GetNormals(fruitSolver, buffers->normals.GetBuffer(), buffers->normals.size());

		BeginLines();

		for (int i = 0; i < buffers->normals.size(); ++i)
		{
			DrawLine(Vec3(buffers->positions[i]),
				Vec3(buffers->positions[i] - buffers->normals[i] * buffers->normals[i].w),
				Vec4(0.0f, 1.0f, 0.0f, 0.0f));
		}

		EndLines();
	}
}