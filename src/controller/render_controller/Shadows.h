#pragma once

#include "../../opengl/shader.h"

struct ShadowMap
{
	GLuint texture;
	GLuint framebuffer;
};

class Shadows {

private:

	ShadowMap *shadowMap;

	// параметры теней
	const int kShadowResolution = 2048;

public:

	Shadows() {}

	ShadowMap* ShadowCreate();
};