#ifndef VIDEO_H
#define VIDEO_H

#include <fruit/controller/render_controller/render_controller.h>
#include <fstream>

namespace FruitWork {
namespace Utilits {

class Video {
public:

	/*! Status of enabling the frame record */
	bool capture = false;

	/*! 
	 * Get FILE of writing frames
	 * @return pointer of FILE
	 */

	FILE* GetFFMpeg() const;

	/*!
	 * Create frame for video
	 * @param[in] width		Width of the window
	 * @paran[in] height	Height of the windows
	 */
	void CreateFrame(uint32_t width, uint32_t height) const;

private:
	
	FILE* ffmpeg = nullptr;
};

}
}

#endif // VIDEO_H