#include <utilits/video.h>
#include <flex/core/tga.h>

bool Video::GetCapture() {
	return capture;
}
void Video::SetCapture(bool capture) {
	this->capture = capture;
}

FILE* Video::GetFFMpeg() const {
	return ffmpeg;
}

void Video::CreateFrame(uint32_t width, uint32_t height) const {
	TgaImage img;
	img.m_data = new uint32_t[width * height];

	ReadFrame(reinterpret_cast<int*>(img.m_data), width, height);

	fwrite(img.m_data, sizeof(uint32_t) * width * height, 1, ffmpeg);

	delete[] img.m_data;
}