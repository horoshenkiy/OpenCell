#pragma once

#include "render_controller/RenderController.h"
#include "render_controller/Camera.h"

// глобальные параметры исполнения
extern bool g_pause;
extern bool g_debug;
extern bool g_reset;
extern bool g_step;

class SDLController {
private:
	// for control of window and camera

	RenderController *renderController = nullptr;
	Camera *camera = nullptr;

	// for control of parameters for flex
	FlexParams *flexParams = nullptr;

	// mouse
	int lastx = -1;
	int lasty = -1;
	int lastb = -1;

	unsigned int windowId = -1;

	// methods of control keys
	///////////////////////////////////////////////////////////////////
	bool InputKeyboardDown(unsigned char key, int x, int y);

	void InputKeyboardUp(unsigned char key, int x, int y);

	// methods of control mouse
	////////////////////////////////////////////////////////////////////

	void MouseFunc(int b, int state, int x, int y);

	void MousePassiveMotionFunc(int x, int y);

	void MouseMotionFunc(unsigned state, int x, int y);

public:
	// инкапсуляция
	unsigned int GetWindowId() const { return windowId; }

	int GetLastX() const { return lastx; }
	void SetLastX(int lastx) { this->lastx = lastx; }

	int GetLastY() const { return lasty; }
	void SetLastY(int lasty) { this->lasty = lasty; }

	int GetLastB() const { return lastb; }
	void SetLastB(int lastb) { this->lastb = lastb; }

	//конструктор и инициализация
	SDLController() {}

	void SDLInit(RenderController *renderController, Camera *camera, FlexParams *flexParams, const char* title);

	// return true, if program is running
	bool SDLMainLoop();

};