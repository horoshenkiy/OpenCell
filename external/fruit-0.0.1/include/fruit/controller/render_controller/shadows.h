#ifndef SHADOWS_H
#define SHADOWS_H

#include "render_gl/shader.h"

struct ShadowMap
{
	GLuint texture;
	GLuint framebuffer;
};

class Shadows {

private:

	ShadowMap *shadowMap = nullptr;

	// параметры теней
	const int kShadowResolution = 2048;

public:

	Shadows() {}

	ShadowMap* ShadowCreate();
};

#endif // SHADOWS_H