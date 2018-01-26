#include "Video.h"

#include "../core/tga.h"

bool Video::GetCapture() {
	return capture;
}
void Video::SetCapture(bool capture) {
	this->capture = capture;
}

FILE* Video::GetFFMpeg() const {
	return ffmpeg;
}

void Video::CreateFrame() const {
	TgaImage img;
	img.m_data = new uint32_t[renderController.GetWidth()*renderController.GetHeight()];

	ReadFrame(reinterpret_cast<int*>(img.m_data), renderController.GetWidth(), renderController.GetHeight());

	fwrite(img.m_data, sizeof(uint32_t)*renderController.GetWidth()*renderController.GetHeight(), 1, ffmpeg);

	delete[] img.m_data;
}