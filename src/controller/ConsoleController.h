#pragma once

#include "../../external/cxxopts-1.4.3/include/cxxopts.hpp"

#include "render_controller/RenderController.h"
#include "imgui_controller/IMGUIController.h"


//extern FlexController *flexController;
extern RenderController renderController;
extern IMGUIController imguiController;

extern bool g_extensions;
extern bool g_benchmark;
extern bool g_teamCity;
//extern bool g_state;

//void ConsoleFun(int argc, char* argv[]);

class ConsoleController {

private:
	cxxopts::Options options { "Open Cell", "Open project of MSU for researching cell's migrations." };
	
public:
	
	ConsoleController() {
		options.add_options()("d,device", "Device for running simulation", cxxopts::value<int>());
		//options.add_options()("m,msaa", "Set MSAA for rendering", cxxopts::value<int>());
		options.add_options()("m,mode", "Set mode (compute or graphic)", cxxopts::value<std::string>());
		options.add_options()("s,state", "Load state for continue simulation", cxxopts::value<std::string>());
		options.add_options()("p,profiling", "Enable profiling");
		options.add_options()("i,interop", "Enable interop");
		options.add_options()("t,tweak", "Enable tweak");
		options.add_options()("f,fullscreen", "Enable fullscreen");
		options.add_options()("h,help", "Print usage info");
	}

	void Parse(int argc, char* argv[]) {
		try {
			options.parse(argc, argv);
			
			if (options.count("help") > 0) {
				std::cout << options.help() << std::endl;
				exit(0);
			}
		}
		catch (cxxopts::OptionParseException &ex) {
			std::cerr << "Error: " << ex.what() << std::endl;
			exit(1);
		}
	}

	void SetParamFlexController(FlexController &flexController) const {
		if (options.count("device") > 0)
			flexController.SetDevice(options["device"].as<int>());

	}

	std::string GetState() const {
		if (options.count("state") > 0)
			return options["state"].as<std::string>();

		return "";
	}
};