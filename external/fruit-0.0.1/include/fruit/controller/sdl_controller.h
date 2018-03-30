#ifndef SDL_CONTROLLER_H
#define SDL_CONTROLLER_H

#include <fruit/controller/render_controller/render_controller.h>
#include <fruit/controller/render_controller/camera.h>

class Serializer;

class SDLController {
public:
	// инкапсул€ци€
	unsigned int GetWindowId() const { return windowId; }

	int GetLastX() const { return lastx; }
	void SetLastX(int lastx) { this->lastx = lastx; }

	int GetLastY() const { return lasty; }
	void SetLastY(int lasty) { this->lasty = lasty; }

	int GetLastB() const { return lastb; }
	void SetLastB(int lastb) { this->lastb = lastb; }

	// initialize
	void SDLInit(RenderController *renderController, 
				 Camera *camera, 
				 const char* title);

	void SDLPostInit(Serializer *serializer);

	// return true, if program is running
	bool SDLMainLoop();

private:

	// methods of control keys
	///////////////////////////////////////////////////////////////////
	bool InputKeyboardDown(unsigned char key, int x, int y);

	void InputKeyboardUp(unsigned char key, int x, int y);

	// methods of control mouse
	////////////////////////////////////////////////////////////////////

	void MouseFunc(int b, int state, int x, int y);

	void MousePassiveMotionFunc(int x, int y);

	void MouseMotionFunc(unsigned state, int x, int y);

	// for control of window and camera
	RenderController *renderController = nullptr;
	Camera *camera = nullptr;

	// for control of parameters for flex
	FlexParams *flexParams = nullptr;

	// for save state
	Serializer *serializer;

	// mouse
	int lastx = -1;
	int lasty = -1;
	int lastb = -1;

	unsigned int windowId = -1;

};

#endif // SDL_CONTROLLER_H