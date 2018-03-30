#ifndef VIDEO_H
#define VIDEO_H

#include <fruit/controller/render_controller/render_controller.h>
#include <fstream>

class Video {

public:

	bool GetCapture();
	void SetCapture(bool capture);

	FILE* GetFFMpeg() const;

	/*!
	 * Create frame for video
	 * @param[in] width		Width of the window
	 * @paran[in] height	Height of the windows
	 */
	void CreateFrame(uint32_t width, uint32_t height) const;

private:
	
	bool capture = false;
	FILE* ffmpeg = nullptr;
};

#endif // VIDEO_H