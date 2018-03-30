#include <application.h>

#include "fruit_core/platform.h"
#include <fruit_extensions/NvFlexImplFruit.h>

using namespace FruitWork;

///////////////////////////////////////////////////////////////////////
// Application params
/////////////////////////////////////////////////////////////////////

bool AppParams::g_reset = false;
bool AppParams::g_pause = false;
bool AppParams::g_step = false;
bool AppParams::g_debug = false;

bool AppParams::g_benchmark = false;

// count of frames
int AppParams::g_frame = 0;

bool AppParams::g_extensions = true;

// logging
bool AppParams::g_teamCity = false;

/////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Application
/////////////////////////////////////////////////////////////////////

// methods
///////////////////////////////////////////////////////////////////////

void Application::Run(int argc, char* argv[]) {
	// Read argument from console
	consoleController.Parse(argc, argv);

	try {
		Initialize();
	}
	catch (std::exception &ex) {
		std::cout << ex.what() << std::endl;
		exit(-1);
	}
	
	MainLoop();
	Shutdown();
}

void Application::MainLoop() {
	bool running = true;
	while (running) {
		UpdateFrame();
		running = sdlController.SDLMainLoop();
	}
}

void Application::Initialize() {
	if (!scene)
		throw std::runtime_error("Scene is n't loaded!");

	// init compute
	InitCompute();

	// init render 
	InitRender();

	// init default scene
	InitSceneCompute();
	InitSceneRender();
}

void Application::InitCompute() {
	// initialize compute controller
	computeController.Initialize(true);

	// it's temp
	flexController = &FlexController::Get();
	flexParams = &FlexParams::Get();
	buffers = &SimBuffers::Get();
}

void Application::InitRender() {
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

void Application::InitSceneCompute() {
	// mapping buffers
	computeController.MapBuffers();

	// initialize scene
	/////////////////////////////////////////////////////////////////
	std::string state = consoleController.GetState();
	if (state == "") {
		scene->Initialize();
	}
	else {
		// create serializer
		//serializer = Serializer(reinterpret_cast<SceneCell*>(scene));

		//scene->InitializeFromFile();
		//serializer.LoadStateBinary(state);
	}

	scene->PostInitialize();
	/////////////////////////////////////////////////////////////////

	// build constaints
	computeController.PostInitialize();

	// unmap so we can start transferring data to GPU
	computeController.UnmapBuffers();

	// initialize solver, set params andsend data to GPU
	computeController.InitializeGPU();
}

void Application::InitSceneRender() {
	// save mesh positions for skinning
	if (renderBuffers->mesh)
		renderBuffers->meshRestPositions = renderBuffers->mesh->m_positions;
	else
		renderBuffers->meshRestPositions.resize(0);

	// center camera on particles
	Vec3 sceneLower = scene->GetSceneLower();
	Vec3 sceneUpper = scene->GetSceneUpper();

	camera.SetCamPos(Vec3((sceneLower.x + sceneUpper.x)*0.5f,
		std::min(sceneUpper.y*1.25f, 6.0f),
		sceneUpper.z + std::min(sceneUpper.y, 6.0f)*2.0f));

	camera.SetCamAngle(Vec3(0.0f, -DegToRad(15.0f), 0.0f));

	// give scene a chance to modify camera position
	scene->CenterCamera();

	// create render buffers
	renderBuffers->fluidRenderBuffers = CreateFluidRenderBuffers(buffers->maxParticles, flexParams->interop);
	imguiController.Initialize(scene, flexController, flexParams, &renderController, renderParam, &sdlController);

	sdlController.SDLPostInit(&serializer);
	renderController.SetScene(scene);

	// create shadow maps
	renderController.SetShadowMap(renderController.shadows.ShadowCreate());
}

void Application::Reset() {
	FlexController &flexController = FlexController::Get();

	if (flexController.GetSolver()) {
		if (buffers)
			buffers->Reset(flexController.GetLib());

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
		flexController.SetSolver(nullptr);

		scene->Reset();
	}

	InitSceneCompute();
	InitSceneRender();
}

void Application::Shutdown() {
	FlexController *flexController = &FlexController::Get();

	for (auto& iter : renderBuffers->meshes) {
		NvFlexDestroyTriangleMesh(flexController->GetLib(), iter.first);
		DestroyGpuMesh(iter.second);
	}

	for (auto& iter : renderBuffers->fields) {
		NvFlexDestroyDistanceField(flexController->GetLib(), iter.first);
		DestroyGpuMesh(iter.second);
	}

	for (auto& iter : renderBuffers->convexes) {
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

void Application::UpdateFrame() {

	static double lastTime = 0;

	// real elapsed frame time
	double frameBeginTime = FruitGetSeconds();
	timer.realdt = float(frameBeginTime - lastTime);
	lastTime = frameBeginTime;

	//-------------------------------------------------------------------
	// Scene Update

	double waitBeginTime = FruitGetSeconds();
	computeController.MapBuffers();
	double waitEndTime = FruitGetSeconds();

	camera.UpdateCamera();

	if (!AppParams::g_pause || AppParams::g_step)
		UpdateScene();

	float newSimLatency = timer.GetDeviceLatency();

	//-------------------------------------------------------------------
	// Render
	float newRenderTime = Render();

	/*if (serializer.GetIsNeedSave()) {
		std::time_t t = std::time(0);
		std::stringstream ss;
		ss << std::put_time(std::localtime(&t), "%Y-%m-%d-%H_%M_%S");

		serializer.SaveStateBinary(ss.str() + "_" + std::to_string(std::rand()));
		serializer.SetIsNeedSave(false);
	}*/

	computeController.UnmapBuffers();

	// if user requested a scene reset process it now
	if (AppParams::g_reset) {
		Reset();
		AppParams::g_reset = false;
	}

	//-------------------------------------------------------------------
	// Flex Update

	double updateBeginTime = FruitGetSeconds();
	computeController.Update();
	double updateEndTime = FruitGetSeconds();

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

void Application::UpdateScene() {
	// give scene a chance to make changes to particle buffers
	scene->Update();
}

float Application::Render() {
	double renderBeginTime = FruitGetSeconds();

	// main render scene //////////////////
	// TODO: move to render controller
	StartFrame(Vec4(renderParam->clearColor, 1.0f));

	renderController.RenderScene(computeController.GetActiveCount(), computeController.GetDiffuseParticles());
	renderController.RenderDebug();

	// TODO: move to render controller
	EndFrame(renderController.GetWidth(), renderController.GetHeight());
	///////////////////////////////////////

	imguiController.DoUI(computeController.GetActiveCount(), computeController.GetDiffuseParticles());

	if (video.GetCapture())
		video.CreateFrame(renderController.GetWidth(), renderController.GetHeight());

	double renderEndTime = FruitGetSeconds();

	return float(renderEndTime - renderBeginTime);
}

// values
///////////////////////////////////////////////////////////////////////

Serializer Application::serializer;

Camera Application::camera;
Scene* Application::scene = nullptr;

Timer Application::timer;

Video Application::video;

ConsoleController Application::consoleController;
IMGUIController Application::imguiController;
RenderController Application::renderController;
SDLController Application::sdlController;
FlexController* Application::flexController;
AbstComputeController& Application::computeController = FruitNvFlex();

SimBuffers* Application::buffers;
RenderBuffers* Application::renderBuffers;

RenderParam* Application::renderParam;
FlexParams* Application::flexParams;

///////////////////////////////////////////////////////////////////////