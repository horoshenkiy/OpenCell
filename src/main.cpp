#include "../core/types.h"
#include "../core/maths.h"
#include "../core/platform.h"
#include "../core/mesh.h"
#include "../core/tga.h"
#include "../core/cloth.h"

#include "../external/SDL2-2.0.4/include/SDL.h"

#include "../fruit_core/platform.h"

#include "../include/NvFlex.h"

#include <iomanip>
#include <iostream>
#include <map>
#include <ctime>

#include "shaders.h"

// buffer
#include "controller/compute_controller/SimBuffers.h"
#include "controller/render_controller/RenderBuffer.h"

// controller
#include "controller/SDLController.h"
#include "controller/ConsoleController.h"
#include "controller/compute_controller/FlexController.h"
#include "controller/render_controller/RenderController.h"
#include "controller/render_controller/Camera.h"
#include "controller/imgui_controller/IMGUIController.h"

// params
#include "controller/render_controller/RenderParam.h"
#include "controller/compute_controller/FlexParams.h"
#include "controller/compute_controller/ComputeController.h"

#include "utilits/Timer.h"
#include "utilits/Video.h"

// my core
#include "../fruit_extensions/NvFlexImplFruitExt.h"

// scene
#include "scenes.h"
#include "scenes\SceneCell.h"

// serializer
Serializer serializer;

// camera
static Camera camera;

// scene
Scene *scene;

//timer
Timer timer;

//video
Video video;

// controllers
RenderController renderController;
SDLController sdlController;
FlexController *flexController;
IMGUIController imguiController;
ComputeController computeController;

// buffers
SimBuffers *g_buffers;
RenderBuffers *renderBuffers;

// parameters
RenderParam *renderParam;
FlexParams *flexParams;

// param of main control //////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool g_reset = false;  //if the user clicks the reset button or presses the reset key this is set to true;
bool g_pause = false;
bool g_step = false;
bool g_debug = false;

bool g_benchmark = false;

// count of frames
int g_frame = 0;

///////////////////////////////////

bool g_extensions = true;

// logging
bool g_teamCity = false;

// download state
bool g_state = false;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void InitCompute() {
	
	RandInit();

	// init flex
	flexController = &FlexController::Instance();

	// initialize params
	flexParams = &FlexParams::Instance();

	// init benchmark 
	if (g_benchmark)
		std::cout << "Compute Device: " << flexController->GetDeviceName() << std::endl;

	// create compute buffer
	g_buffers = &SimBuffers::Instance(flexController->GetLib());
}

void InitRender() {
	// create render buffer
	renderBuffers = &RenderBuffers::Instance();

	// init render param
	renderParam = &RenderParam::Instance();

	// init controller
	sdlController.SDLInit(&renderController, &camera, "Open Cell");
	if (renderController.GetFullscreen())
		SDL_SetWindowFullscreen(renderController.GetWindow(), SDL_WINDOW_FULLSCREEN_DESKTOP);

	// init gl
	renderController.InitRender(&camera);
}

void InitSceneCompute(Scene *&scene) {
	
	scene = new SceneCell("Cell motility!");

	// initialize scene
	/////////////////////////////////////////////////////////////////
	g_buffers->MapBuffers();
	
	if (!g_state) {
		scene->Initialize(flexController, flexParams);
	}
	else {
		// create serializer
		serializer = Serializer((SceneCell*)scene, g_buffers, renderBuffers);

		scene->InitializeFromFile(flexController, flexParams);
		serializer.LoadStateBinary("123");
		
	}
	
	scene->PostInitialize();
	//////////////////////////////////////////////////////////////////

	g_buffers->PostInitialize();

	// main create method for the Flex solver
	NvFlexSolver *solver = NvFlexCreateSolver(flexController->GetLib(),
		g_buffers->maxParticles,
		g_buffers->maxDiffuseParticles,
		flexParams->maxNeighborsPerParticle);
	flexController->SetSolver(solver);

	// build constraints
	g_buffers->BuildConstraints();

	// unmap so we can start transferring data to GPU
	g_buffers->UnmapBuffers();

	// params of Flex
	NvFlexSetParams(solver, &(flexParams->params));

	// send buffers
	g_buffers->SendBuffers(solver);

	// 	initialise compute controller
	computeController.Initialize(flexController, flexParams, renderParam, scene);
}

void InitSceneRender(Scene *scene, bool centerCamera = true) {
	// save mesh positions for skinning
	if (renderBuffers->mesh)
		renderBuffers->meshRestPositions = renderBuffers->mesh->m_positions;
	else
		renderBuffers->meshRestPositions.resize(0);

	// center camera on particles
	if (centerCamera)
	{
		Vec3 sceneLower = scene->GetSceneLower();
		Vec3 sceneUpper = scene->GetSceneUpper();

		camera.SetCamPos(Vec3((sceneLower.x + sceneUpper.x)*0.5f,
			std::min(sceneUpper.y*1.25f, 6.0f),
			sceneUpper.z + std::min(sceneUpper.y, 6.0f)*2.0f));

		camera.SetCamAngle(Vec3(0.0f, -DegToRad(15.0f), 0.0f));

		// give scene a chance to modify camera position
		scene->CenterCamera();
	}

	// create render buffers
	renderBuffers->fluidRenderBuffers = CreateFluidRenderBuffers(g_buffers->maxParticles, flexParams->interop);
	imguiController.Initialize(scene, flexController, flexParams, &renderController, renderParam, &sdlController);

	sdlController.SDLPostInit(&serializer);
	renderController.SetScene(scene);

	// create shadow maps
	renderController.SetShadowMap(renderController.shadows.ShadowCreate());
}

void Initialize() {
	// init compute
	InitCompute();

	// init render 
	InitRender();

	// init default scene
	InitSceneCompute(scene);
	InitSceneRender(scene);
}

void Reset() {
	FlexController &flexController = FlexController::Instance();

	if (flexController.GetSolver())
	{
		if (g_buffers)
			g_buffers->Reset(flexController.GetLib());

		DestroyFluidRenderBuffers(renderBuffers->fluidRenderBuffers);

		for (auto& iter : renderBuffers->meshes) {
			NvFlexDestroyTriangleMesh(flexController.GetLib(), iter.first);
			DestroyGpuMesh(iter.second);
		}

		for (auto& iter : renderBuffers->fields) {
			NvFlexDestroyDistanceField(flexController.GetLib(), iter.first);
			DestroyGpuMesh(iter.second);
		}

		for (auto& iter : renderBuffers->convexes) {
			NvFlexDestroyConvexMesh(flexController.GetLib(), iter.first);
			DestroyGpuMesh(iter.second);
		}

		renderBuffers->fields.clear();
		renderBuffers->meshes.clear();
		renderBuffers->convexes.clear();

		delete renderBuffers->mesh;
		renderBuffers->mesh = new Mesh();

		NvFlexDestroySolver(flexController.GetSolver());
		flexController.SetSolver(NULL);

		delete scene;
	}

	InitSceneCompute(scene);
	InitSceneRender(scene, false);
}

void Shutdown()
{
	FlexController *flexController = &FlexController::Instance();

	for (auto& iter : renderBuffers->meshes)
	{
		NvFlexDestroyTriangleMesh(flexController->GetLib(), iter.first);
		DestroyGpuMesh(iter.second);
	}

	for (auto& iter : renderBuffers->fields)
	{
		NvFlexDestroyDistanceField(flexController->GetLib(), iter.first);
		DestroyGpuMesh(iter.second);
	}

	for (auto& iter : renderBuffers->convexes)
	{
		NvFlexDestroyConvexMesh(flexController->GetLib(), iter.first);
		DestroyGpuMesh(iter.second);
	}

	renderBuffers->fields.clear();
	renderBuffers->meshes.clear();

	NvFlexDestroySolver(flexController->GetSolver());
	NvFlexShutdown(flexController->GetLib());

	if (renderController.GetFluidRenderer())
		DestroyFluidRenderer(renderController.GetFluidRenderer());

	DestroyFluidRenderBuffers(renderBuffers->fluidRenderBuffers);

	ShadowDestroy(renderController.GetShadowMap());
	DestroyRender();

	SDL_DestroyWindow(renderController.GetWindow());
	SDL_Quit();
}

void UpdateScene() {
	// give scene a chance to make changes to particle buffers
	scene->Update();
}

float Render() {
	double renderBeginTime = GetSeconds();

	// main render scene //////////////////
	StartFrame(Vec4(renderParam->clearColor, 1.0f));

	renderController.RenderScene(computeController.GetActiveCount(), computeController.GetDiffuseParticles());
	renderController.RenderDebug();

	EndFrame();
	///////////////////////////////////////

	imguiController.DoUI(computeController.GetActiveCount(), computeController.GetDiffuseParticles());

	if (video.GetCapture())
		video.CreateFrame();

	double renderEndTime = GetSeconds();

	return float(renderEndTime - renderBeginTime);
}

void UpdateFrame() {

	static double lastTime = 0;

	// real elapsed frame time
	double frameBeginTime = FruitGetSeconds();
	timer.realdt = float(frameBeginTime - lastTime);
	lastTime = frameBeginTime;

	//-------------------------------------------------------------------
	// Scene Update

	double waitBeginTime = FruitGetSeconds();
	g_buffers->MapBuffers();
	double waitEndTime = FruitGetSeconds();

	camera.UpdateCamera();

	if (!g_pause || g_step)
		UpdateScene();

	float newSimLatency = timer.GetDeviceLatency();

	//-------------------------------------------------------------------
	// Render
	float newRenderTime = Render();

	if (serializer.GetIsNeedSave()) {
		std::time_t t = std::time(0);
		std::stringstream ss;
		ss << std::put_time(std::localtime(&t), "%Y-%m-%d-%H_%M_%S");

		serializer.SaveStateBinary(ss.str() + "_" + std::to_string(std::rand()));
		serializer.SetIsNeedSave(false);
	}

	g_buffers->UnmapBuffers();

	// if user requested a scene reset process it now
	if (g_reset) {
		Reset();
		g_reset = false;
	}

	//-------------------------------------------------------------------
	// Flex Update

	double updateBeginTime = FruitGetSeconds();
	computeController.Update();	
	double updateEndTime = GetSeconds();

	//-------------------------------------------------------
	// Update the on-screen timers

	float newUpdateTime = float(updateEndTime - updateBeginTime);
	float newWaitTime = float(waitEndTime - waitBeginTime);

	// Exponential filter to make the display easier to read
	const float timerSmoothing = 0.05f;

	timer.updateTime = (timer.updateTime == 0.0f) ? newUpdateTime : Lerp(timer.updateTime, newUpdateTime, timerSmoothing);
	timer.renderTime = (timer.renderTime == 0.0f) ? newRenderTime : Lerp(timer.renderTime, newRenderTime, timerSmoothing);
	timer.waitTime = (timer.waitTime == 0.0f) ? newWaitTime : Lerp(timer.waitTime, newWaitTime, timerSmoothing);
	timer.simLatency = (timer.simLatency == 0.0f) ? newSimLatency : Lerp(timer.simLatency, newSimLatency, timerSmoothing);
	
	PresentFrame(renderParam->vsync);
}

void MainLoop()
{
	bool running = true;
	while (running) {
		UpdateFrame();
		running = sdlController.SDLMainLoop();
	}
}

int main(int argc, char* argv[]) {
	// Read argument from console
	ConsoleController(argc, argv);

	Initialize();
	MainLoop();
	Shutdown();

	return 0;
}
