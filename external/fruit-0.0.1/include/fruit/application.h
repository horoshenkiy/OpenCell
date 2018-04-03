#ifndef APPLICATION_H
#define APPLICATION_H

#include <fruit/controller/compute_controller/compute_controller.h>
#include <fruit/controller/console_controller.h>
#include <fruit/controller/render_controller/camera.h>

#include <fruit/utilits/serializer.h>
#include <fruit/utilits/timer.h>
#include <fruit/utilits/video.h>
#include "fruit/scene.h"

namespace FruitWork {

	class Application {
	public:

		/* ! Load scene for simulation */
		static void LoadScene(Scene *_scene) { scene = _scene; }

		/* ! Run Fruit framework */
		static void Run(int argc, char* argv[]);

		// TODO: Add singleton for compute controller
		static Compute::ComputeController &computeController;

	private:

		/* ! Main looper of application */
		static void MainLoop();

		static void InitCompute();
		static void InitRender();

		static void InitSceneCompute();
		static void InitSceneRender();

		static void Initialize();

		static void Reset();
		static void Shutdown();

		static void UpdateFrame();
		static void UpdateScene();
		
		static void Render();

		// values 
		////////////////////////////////////////////////////////

		// scene
		static Scene *scene;

		// controllers

		static Render::RenderController renderController;
		static IMGUI::IMGUIController imguiController;
		static Control::SDLController sdlController;
		static Control::ConsoleController consoleController;

		// buffers
		static Compute::SimBuffers *buffers;
		static Render::RenderBuffers *renderBuffers;

		//parameters
		static Compute::FlexParams *flexParams;
		static Render::RenderParam *renderParam;

		// camera
		static Render::Camera camera;

		//timer
		static Utilits::Timer *timer;

		//video
		static Utilits::Video video;

		// serializer
		static Serializer serializer;

		////////////////////////////////////////////////////////
	};

	struct AppParams {

		//if the user clicks the reset button or presses the reset key this is set to true;
		static bool g_reset;  
		static bool g_pause;
		static bool g_step;
		static bool g_debug;

		static bool g_benchmark;

		// count of frames
		static int g_frame;

		static bool g_extensions;

		// logging
		static bool g_teamCity;

	};
}

#endif // APPLICATION_H