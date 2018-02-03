#include "IMGUIController.h"

// do statistic and menu 
//////////////////////////////////////////////
void IMGUIController::DoStatistic(int numParticles, int numDiffuse) {

	x += 180;
	fontHeight = 13;

	imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Frame: %d", g_frame); y -= fontHeight * 2;

	if (!video.GetFFMpeg())
	{
		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Frame Time: %.2fms", timer.realdt*1000.0f); y -= fontHeight * 2;

		// If detailed profiling is enabled, then these timers will contain the overhead of the detail timers, so we won't display them.
		if (!flexParams->profile)
		{
			imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Sim Time (CPU): %.2fms", timer.updateTime*1000.0f); y -= fontHeight;
			imguiDrawString(x, y, 0.97f, 0.59f, 0.27f, IMGUI_ALIGN_RIGHT, "Sim Latency (GPU): %.2fms", timer.simLatency); y -= fontHeight * 2;
		}
		else
		{
			y -= fontHeight * 3;
		}
	}

	imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Particle Count: %d", numParticles); y -= fontHeight;
	imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Diffuse Count: %d", numDiffuse); y -= fontHeight;
	imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Rigid Count: %d", buffers->rigidOffsets.size() > 0 ? buffers->rigidOffsets.size() - 1 : 0); y -= fontHeight;
	imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Spring Count: %d", buffers->springLengths.size()); y -= fontHeight;
	imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Num Substeps: %d", flexParams->numSubsteps); y -= fontHeight;
	imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Num Iterations: %d", flexParams->params.numIterations); y -= fontHeight * 2;

	imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Device: %s", flexController->GetDeviceName()); y -= fontHeight * 2;

	if (flexParams->profile)
	{
		imguiDrawString(x, y, 0.97f, 0.59f, 0.27f, IMGUI_ALIGN_RIGHT, "Total GPU Sim Latency: %.2fms", flexController->GetTimers().total); y -= fontHeight * 2;

		imguiDrawString(x, y, 0.0f, 1.0f, 0.0f, IMGUI_ALIGN_RIGHT, "GPU Latencies"); y -= fontHeight;

		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Predict: %.2fms", flexController->GetTimers().predict); y -= fontHeight;
		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Create Cell Indices: %.2fms", flexController->GetTimers().createCellIndices); y -= fontHeight;
		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Sort Cell Indices: %.2fms", flexController->GetTimers().sortCellIndices); y -= fontHeight;
		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Reorder: %.2fms", flexController->GetTimers().reorder); y -= fontHeight;
		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "CreateGrid: %.2fms", flexController->GetTimers().createGrid); y -= fontHeight;
		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Collide Particles: %.2fms", flexController->GetTimers().collideParticles); y -= fontHeight;
		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Collide Shapes: %.2fms", flexController->GetTimers().collideShapes); y -= fontHeight;
		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Collide Triangles: %.2fms", flexController->GetTimers().collideTriangles); y -= fontHeight;
		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Calculate Density: %.2fms", flexController->GetTimers().calculateDensity); y -= fontHeight;
		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Solve Densities: %.2fms", flexController->GetTimers().solveDensities); y -= fontHeight;
		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Solve Velocities: %.2fms", flexController->GetTimers().solveVelocities); y -= fontHeight;
		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Solve Rigids: %.2fms", flexController->GetTimers().solveShapes); y -= fontHeight;
		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Solve Springs: %.2fms", flexController->GetTimers().solveSprings); y -= fontHeight;
		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Solve Inflatables: %.2fms", flexController->GetTimers().solveInflatables); y -= fontHeight;
		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Solve Contacts: %.2fms", flexController->GetTimers().solveContacts); y -= fontHeight;
		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Apply Deltas: %.2fms", flexController->GetTimers().applyDeltas); y -= fontHeight;
		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Finalize: %.2fms", flexController->GetTimers().finalize); y -= fontHeight;
		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Update Triangles: %.2fms", flexController->GetTimers().updateTriangles); y -= fontHeight;
		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Update Normals: %.2fms", flexController->GetTimers().updateNormals); y -= fontHeight;
		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Update Bounds: %.2fms", flexController->GetTimers().updateBounds); y -= fontHeight;
		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Calculate Anisotropy: %.2fms", flexController->GetTimers().calculateAnisotropy); y -= fontHeight;
		imguiDrawString(x, y, 1.0f, 1.0f, 1.0f, IMGUI_ALIGN_RIGHT, "Update Diffuse: %.2fms", flexController->GetTimers().updateDiffuse); y -= fontHeight * 2;
	}

	x -= 180;
}

void IMGUIController::DoMenu() {

	int uiOffset = 250;
	int uiBorder = 20;
	int uiWidth = 200;
	int uiHeight = renderController->GetHeight() - uiOffset - uiBorder * 3;
	int uiLeft = uiBorder;

	if (tweakPanel)
	{
		static int scroll = 0;

		imguiBeginScrollArea("Options", uiLeft, renderController->GetHeight() - uiBorder - uiHeight - uiBorder, uiWidth, uiHeight, &scroll);
		imguiSeparatorLine();

		// global options
		imguiLabel("Global");

		if (imguiCheck("Pause", g_pause))
			g_pause = !g_pause;

		imguiSeparatorLine();

		if (imguiCheck("Wireframe", renderParam->wireframe))
			renderParam->wireframe = !renderParam->wireframe;

		if (imguiCheck("Draw Points", renderParam->drawPoints))
			renderParam->drawPoints = !renderParam->drawPoints;

		if (imguiCheck("Draw Fluid", renderParam->drawEllipsoids))
			renderParam->drawEllipsoids = !renderParam->drawEllipsoids;

		if (imguiCheck("Draw Mesh", renderParam->drawMesh))
		{
			renderParam->drawMesh = !renderParam->drawMesh;
			//g_drawRopes = !g_drawRopes;
		}

		if (imguiCheck("Draw Cloth", renderParam->drawCloth))
			renderParam->drawCloth = !renderParam->drawCloth;

		if (imguiCheck("Draw Basis", renderParam->drawBases))
			renderParam->drawBases = !renderParam->drawBases;

		if (imguiCheck("Draw Springs", bool(renderParam->drawSprings != 0)))
			renderParam->drawSprings = ((renderParam->drawSprings) ? 0 : 1);

		if (imguiCheck("Draw Contacts", renderParam->drawContacts))
			renderParam->drawContacts = !renderParam->drawContacts;

		imguiSeparatorLine();

		// scene options
		scene->DoGui();

		if (imguiButton("Reset Scene"))
			g_reset = true;

		imguiSeparatorLine();

		float n = float(flexParams->numSubsteps);
		if (imguiSlider("Num Substeps", &n, 1, 10, 1))
			flexParams->numSubsteps = int(n);

		n = float(flexParams->params.numIterations);
		if (imguiSlider("Num Iterations", &n, 1, 20, 1))
			flexParams->params.numIterations = int(n);

		imguiSeparatorLine();
		imguiSlider("Gravity X", &flexParams->params.gravity[0], -50.0f, 50.0f, 1.0f);
		imguiSlider("Gravity Y", &flexParams->params.gravity[1], -50.0f, 50.0f, 1.0f);
		imguiSlider("Gravity Z", &flexParams->params.gravity[2], -50.0f, 50.0f, 1.0f);

		imguiSeparatorLine();
		imguiSlider("Radius", &flexParams->params.radius, 0.01f, 0.5f, 0.01f);
		imguiSlider("Solid Radius", &flexParams->params.solidRestDistance, 0.0f, 0.5f, 0.001f);
		imguiSlider("Fluid Radius", &flexParams->params.fluidRestDistance, 0.0f, 0.5f, 0.001f);

		// common params
		imguiSeparatorLine();
		imguiSlider("Dynamic Friction", &flexParams->params.dynamicFriction, 0.0f, 1.0f, 0.01f);
		imguiSlider("Static Friction", &flexParams->params.staticFriction, 0.0f, 1.0f, 0.01f);
		imguiSlider("Particle Friction", &flexParams->params.particleFriction, 0.0f, 1.0f, 0.01f);
		imguiSlider("Restitution", &flexParams->params.restitution, 0.0f, 1.0f, 0.01f);
		imguiSlider("SleepThreshold", &flexParams->params.sleepThreshold, 0.0f, 1.0f, 0.01f);
		imguiSlider("Shock Propagation", &flexParams->params.shockPropagation, 0.0f, 10.0f, 0.01f);
		imguiSlider("Damping", &flexParams->params.damping, 0.0f, 10.0f, 0.01f);
		imguiSlider("Dissipation", &flexParams->params.dissipation, 0.0f, 0.01f, 0.0001f);
		imguiSlider("SOR", &flexParams->params.relaxationFactor, 0.0f, 5.0f, 0.01f);

		imguiSlider("Collision Distance", &flexParams->params.collisionDistance, 0.0f, 0.5f, 0.001f);
		imguiSlider("Collision Margin", &flexParams->params.shapeCollisionMargin, 0.0f, 5.0f, 0.01f);

		// rigid params
		imguiSeparatorLine();
		imguiSlider("Plastic Creep", &flexParams->params.plasticCreep, 0.0f, 1.0f, 0.001f);
		imguiSlider("Plastic Threshold", &flexParams->params.plasticThreshold, 0.0f, 0.5f, 0.001f);

		// cloth params
		imguiSeparatorLine();
		imguiSlider("Drag", &flexParams->params.drag, 0.0f, 1.0f, 0.01f);
		imguiSlider("Lift", &flexParams->params.lift, 0.0f, 1.0f, 0.01f);
		imguiSeparatorLine();

		// fluid params
		if (imguiCheck("Fluid", flexParams->params.fluid))
			flexParams->params.fluid = !flexParams->params.fluid;

		imguiSlider("Adhesion", &flexParams->params.adhesion, 0.0f, 10.0f, 0.01f);
		imguiSlider("Cohesion", &flexParams->params.cohesion, 0.0f, 0.2f, 0.0001f);
		imguiSlider("Surface Tension", &flexParams->params.surfaceTension, 0.0f, 50.0f, 0.01f);
		imguiSlider("Viscosity", &flexParams->params.viscosity, 0.0f, 120.0f, 0.01f);
		imguiSlider("Vorticicty Confinement", &flexParams->params.vorticityConfinement, 0.0f, 120.0f, 0.1f);
		imguiSlider("Solid Pressure", &flexParams->params.solidPressure, 0.0f, 1.0f, 0.01f);
		imguiSlider("Surface Drag", &flexParams->params.freeSurfaceDrag, 0.0f, 1.0f, 0.01f);
		imguiSlider("Buoyancy", &flexParams->params.buoyancy, -1.0f, 1.0f, 0.01f);

		imguiSeparatorLine();
		imguiSlider("Anisotropy Scale", &flexParams->params.anisotropyScale, 0.0f, 30.0f, 0.01f);
		imguiSlider("Smoothing", &flexParams->params.smoothing, 0.0f, 1.0f, 0.01f);

		// diffuse params
		imguiSeparatorLine();
		imguiSlider("Diffuse Threshold", &flexParams->params.diffuseThreshold, 0.0f, 1000.0f, 1.0f);
		imguiSlider("Diffuse Buoyancy", &flexParams->params.diffuseBuoyancy, 0.0f, 2.0f, 0.01f);
		imguiSlider("Diffuse Drag", &flexParams->params.diffuseDrag, 0.0f, 2.0f, 0.01f);
		imguiSlider("Diffuse Scale", &renderParam->diffuseScale, 0.0f, 1.5f, 0.01f);
		imguiSlider("Diffuse Alpha", &renderParam->diffuseColor.w, 0.0f, 3.0f, 0.01f);
		imguiSlider("Diffuse Inscatter", &renderParam->diffuseInscatter, 0.0f, 2.0f, 0.01f);
		imguiSlider("Diffuse Outscatter", &renderParam->diffuseOutscatter, 0.0f, 2.0f, 0.01f);
		imguiSlider("Diffuse Motion Blur", &renderParam->diffuseMotionScale, 0.0f, 5.0f, 0.1f);

		n = float(flexParams->params.diffuseBallistic);
		if (imguiSlider("Diffuse Ballistic", &n, 1, 40, 1))
			flexParams->params.diffuseBallistic = int(n);

		imguiEndScrollArea();
	}
}

// render GUI
////////////////////////////////////////////////
void IMGUIController::Draw()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE3);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE4);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_CUBE_MAP);
	glActiveTexture(GL_TEXTURE5);
	glDisable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE0);

	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDisable(GL_POINT_SPRITE);

	// save scene camera transform
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	const Matrix44 ortho = OrthographicMatrix(0.0f, float(renderController->GetWidth()), 0.0f, float(renderController->GetHeight()), -1.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadMatrixf(ortho);

	glUseProgram(0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	imguiRenderGLDraw();

	// restore camera transform (for picking)
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

// initialize
/////////////////////////////////////////////////////
void IMGUIController::Initialize(Scene *scene,
								 FlexController *flexController, 
								 FlexParams *flexParams, 
								 SimBuffers *buffers,
								 RenderController *renderController, 
								 RenderParam *renderParam,
								 SDLController *sdlController) {

	this->scene = scene;

	this->flexController = flexController;
	this->flexParams = flexParams;
	this->buffers = buffers;

	this->renderController = renderController;
	this->renderParam = renderParam;

	this->sdlController = sdlController;

}

// main method
////////////////////////////////////////////////////////
int IMGUIController::DoUI(int numParticles, int numDiffuse) {
	// gui may set a new scene
	int newScene = -1;

	if (renderParam->showGUI)
	{
		x = renderController->GetWidth() - 200;
		y = renderController->GetHeight() - 23;

		// imgui
		unsigned char button = 0;
		if (sdlController->GetLastB() == SDL_BUTTON_LEFT)
			button = IMGUI_MBUT_LEFT;
		else if (sdlController->GetLastB() == SDL_BUTTON_RIGHT)
			button = IMGUI_MBUT_RIGHT;

		imguiBeginFrame(sdlController->GetLastX(), renderController->GetHeight() - sdlController->GetLastY(), button, 0);

		DoStatistic(numParticles, numDiffuse);
		DoMenu();

		imguiEndFrame();

		// kick render commands
		Draw();
	}

	return newScene;
}