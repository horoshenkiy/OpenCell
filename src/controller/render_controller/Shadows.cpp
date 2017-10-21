#include "Shadows.h"

ShadowMap* Shadows::ShadowCreate()
{
	GLuint texture;
	GLuint framebuffer;

	glVerify(glGenFramebuffers(1, &framebuffer));
	glVerify(glGenTextures(1, &texture));
	glVerify(glBindTexture(GL_TEXTURE_2D, texture));

	glVerify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	glVerify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	// This is to allow usage of shadow2DProj function in the shader 
	glVerify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE));
	glVerify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL));
	glVerify(glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY));

	glVerify(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, kShadowResolution, kShadowResolution, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL));

	glVerify(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));

	glVerify(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0));

	ShadowMap* map = new ShadowMap();
	map->texture = texture;
	map->framebuffer = framebuffer;

	return map;

}