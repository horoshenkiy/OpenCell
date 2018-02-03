#include "ConsoleController.h"

void ConsoleController(int argc, char* argv[]) {
	// process command line args
	for (int i = 1; i < argc; ++i)
	{
		int d;
		if (sscanf(argv[i], "-device=%d", &d))
			flexController.SetDevice(d);

		if (sscanf(argv[i], "-extensions=%d", &d))
			g_extensions = d != 0;

		if (strstr(argv[i], "-benchmark")) {
			g_benchmark = true;
			flexParams.profile = true;
		}

		if (strstr(argv[i], "-tc"))
			g_teamCity = true;

		if (sscanf(argv[i], "-msaa=%d", &d))
			renderParam->msaaSamples = d;

		if (strstr(argv[i], "-fullscreen"))
			renderController.SetFullscreen(true);

		if (sscanf(argv[i], "-vsync=%d", &d))
			renderParam->vsync = d != 0;

		if (sscanf(argv[i], "-multiplier=%d", &d) == 1)
		{
			g_buffers->numExtraMultiplier = d;
		}

		if (strstr(argv[i], "-disabletweak"))
		{
			imguiController.SetTweakPanel(false);
		}

		if (strstr(argv[i], "-disableinterop"))
		{
			flexParams.interop = false;
		}
	}
}