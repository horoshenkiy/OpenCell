#ifndef APPLICATION_H
#define APPLICATION_H

#include <fruit/abst_compute_controller.h>
#include <fruit/controller/console_controller.h>
#include <fruit/controller/render_controller/camera.h>

#include <fruit/utilits/serializer.h>
#include <fruit/utilits/timer.h>
#include <fruit/utilits/video.h>
#include "fruit/scene.h"

namespace FruitWork {

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

	class Application {
	public:

		static void LoadScene(Scene *_scene) {
			scene = _scene;
		}

		static void MainLoop();
		static void Run(int argc, char* argv[]);

		static void InitCompute();
		static void InitRender();

		static void InitSceneCompute();
		static void InitSceneRender();

		static void Initialize();

		static void Reset();
		static void Shutdown();

		static void UpdateFrame();
		static void UpdateScene();
		
		static float Render();

		// serializer
		static Serializer serializer;

		// camera
		static Camera camera;

		// scene
		static Scene *scene;

		//timer
		static Timer timer;

		//video
		static Video video;

		// controllers
		static ConsoleController consoleController;
		static IMGUIController imguiController;
		static RenderController renderController;
		static SDLController sdlController;
		static FlexController *flexController;
		static AbstComputeController &computeController;

		// buffers
		static SimBuffers *buffers;
		static RenderBuffers *renderBuffers;

		//parameters
		static RenderParam *renderParam;
		static FlexParams *flexParams;


	private:
		
	};
}

#endif // APPLICATION_H