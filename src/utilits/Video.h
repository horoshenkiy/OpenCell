#pragma once

#include <fstream>

#include "../controller/render_controller/RenderController.h"
extern RenderController renderController;

class Video {

public:

	bool GetCapture();
	void SetCapture(bool capture);

	FILE* GetFFMpeg() const;

	void CreateFrame() const;

private:
	
	bool capture = false;
	FILE* ffmpeg = nullptr;
};

