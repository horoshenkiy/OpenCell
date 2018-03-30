#ifndef CONSOLE_CONTROLLER_H
#define CONSOLE_CONTROLLER_H

#include <cxxopts.hpp>
#include <fruit/controller/render_controller/render_controller.h>
#include <fruit/controller/imgui_controller/imgui_controller.h>

class ConsoleController {
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

private:
	cxxopts::Options options{ "Open Cell", "Open project of MSU for researching cell's migrations." };
};

#endif // CONSOLE_CONTROLLER_H