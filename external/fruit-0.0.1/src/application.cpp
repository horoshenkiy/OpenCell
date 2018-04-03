#include <application.h>

#include "fruit_core/platform.h"
#include "controller/compute_controller/flex_compute_controller.h"

namespace FruitWork {

using namespace Compute;

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

	// TODO: add method InitUtilits()
	timer = &Utilits::Timer::Get();

	// init default scene
	InitSceneCompute();
	InitSceneRender();
}

void Application::InitCompute() {
	// initialize compute controller
	computeController.Initialize(true);

	// it's temp
	flexParams = &FlexParams::Get();
	buffers = &SimBuffers::Get();
}

void Application::InitRender() {
	// create render buffer
	renderBuffers = &Render::RenderBuffers::Instance();

	// init render param
	renderParam = &Render::RenderParam::Instance();

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

	// create render buffers	
	renderBuffers->Initialize(buffers->maxParticles, flexParams->interop);

	// camera positioning
	////////////////////////////////////////////////////////////////////////////////////////
	
	// center camera on particles
	camera.Initialize(scene->GetSceneLower(), scene->GetSceneUpper());

	// give scene a chance to modify camera position
	scene->CenterCamera();

	////////////////////////////////////////////////////////////////////////////////////////
	
	// initialize values
	imguiController.Initialize(scene, flexParams, &renderController, renderParam, &sdlController);

	// add serializer
	sdlController.SDLPostInit(&serializer);

	// create shadow maps
	renderController.PostInitRender(scene);
}

void Application::Reset() {
	if (computeController.IsInitialize()) {
		computeController.Reset();
		renderController.Reset();
		scene->Reset();
	}

	InitSceneCompute();
	InitSceneRender();
}

void Application::Shutdown() {
	computeController.Destroy();
	renderController.DestroyRender();
	sdlController.SDLDestroy();
}

void Application::UpdateFrame() {

	static double lastTime = 0;

	// real elapsed frame time
	double frameBeginTime = Core::FruitGetSeconds();
	timer->realdt = float(frameBeginTime - lastTime);
	lastTime = frameBeginTime;

	// map buffers
	timer->waitTime = Utilits::TimeMeasurement(computeController, &ComputeController::MapBuffers);
	
	// get time of GPU simulation
	timer->simLatency = timer->GetDeviceLatency();

	//-------------------------------------------------------------------
	// Scene Update

	camera.UpdateCamera();
	if (!AppParams::g_pause || AppParams::g_step)
		UpdateScene();

	//-------------------------------------------------------------------
	// Render
	
	timer->renderTime = Utilits::TimeMeasurement(&Application::Render);

	//-------------------------------------------------------------------
	// Serialize state

	/*if (serializer.GetIsNeedSave()) {
		std::time_t t = std::time(0);
		std::stringstream ss;
		ss << std::put_time(std::localtime(&t), "%Y-%m-%d-%H_%M_%S");

		serializer.SaveStateBinary(ss.str() + "_" + std::to_string(std::rand()));
		serializer.SetIsNeedSave(false);
	}*/

	// unmap buffers
	computeController.UnmapBuffers();

	// if user requested a scene reset process it now
	if (AppParams::g_reset) {
		Reset();
		AppParams::g_reset = false;
	}

	//-------------------------------------------------------------------
	// Flex Update
	timer->updateTime = Utilits::TimeMeasurement(computeController, &ComputeController::Update);

	// temp
	Render::GL::PresentFrame(renderParam->vsync, renderController.GetWindow());
}

void Application::UpdateScene() {
	// give scene a chance to make changes to particle buffers
	scene->Update();
}

void Application::Render() {
	// main render scene //////////////////
	renderController.Render(computeController.GetActiveCount(), computeController.GetDiffuseParticles());

	// render ui
	imguiController.DoUI(computeController.GetActiveCount(), computeController.GetDiffuseParticles());

	if (video.capture)
		video.CreateFrame(renderController.screenWidth, renderController.screenHeight);
}

// values
///////////////////////////////////////////////////////////////////////

Serializer Application::serializer;

Render::Camera Application::camera;
Scene* Application::scene = nullptr;

Utilits::Timer* Application::timer;

Utilits::Video Application::video;

Control::ConsoleController Application::consoleController;
IMGUI::IMGUIController Application::imguiController;
Render::RenderController Application::renderController;
Control::SDLController Application::sdlController;
Compute::ComputeController& Application::computeController = FlexComputeController();

Compute::SimBuffers* Application::buffers;
Render::RenderBuffers* Application::renderBuffers;

Render::RenderParam* Application::renderParam;
Compute::FlexParams* Application::flexParams;

///////////////////////////////////////////////////////////////////////

}
