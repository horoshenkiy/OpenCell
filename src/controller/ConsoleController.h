#pragma once

#include <iostream>

#include "render_controller/RenderController.h"
#include "compute_controller/FlexController.h"
#include "imgui_controller/IMGUIController.h"

#include "render_controller/RenderParam.h"
#include "compute_controller/SimBuffers.h"

//extern FlexController *flexController;
extern RenderController renderController;
extern IMGUIController imguiController;

extern bool g_extensions;
extern bool g_benchmark;
extern bool g_teamCity;
extern bool g_state;

void ConsoleController(int argc, char* argv[]);