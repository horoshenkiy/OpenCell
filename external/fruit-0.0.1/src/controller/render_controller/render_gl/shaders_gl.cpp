// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 20132017 NVIDIA Corporation. All rights reserved.

#include <controller/render_controller/render_controller.h>
#include <controller/render_controller/render_buffer.h>
#include <controller/render_controller/render_gl/shaders.h>
#include <controller/render_controller/render_gl/shader.h>
#include <controller/render_controller/shadows.h>
#include <controller/imgui_controller/imgui_render_gl.h>

#include <flex/core/mesh.h>
#include <flex/core/tga.h>	
#include <flex/core/platform.h>
#include <flex/core/extrude.h>

#include <SDL.h>
#include "controller/compute_controller/compute_controller.h"

#define CudaCheck(x) { cudaError_t err = x; if (err != cudaSuccess) { printf("Cuda error: %d in %s at %s:%d\n", err, #x, __FILE__, __LINE__); assert(0); } }

#include <controller/compute_controller/flex_controller.h>
#include <controller/render_controller/render_param.h>

namespace {

static float gSpotMin = 0.5f;
static float gSpotMax = 1.0f;
float gShadowBias = 0.05f;

} // anonymous namespace

Colour gColors[] =
{
	Colour(0.0f, 0.5f, 1.0f),
	Colour(0.797f, 0.354f, 0.000f),
	Colour(0.092f, 0.465f, 0.820f),
	Colour(0.000f, 0.349f, 0.173f),
	Colour(0.875f, 0.782f, 0.051f),
	Colour(0.000f, 0.170f, 0.453f),
	Colour(0.673f, 0.111f, 0.000f),
	Colour(0.612f, 0.194f, 0.394f)
};

void DestroyRender() {}

void StartFrame(Vec4 clearColor)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);

	glPointSize(5.0f);

	glVerify(glBindFramebuffer(GL_DRAW_FRAMEBUFFER_EXT, FruitWork::Application::renderParam->msaaFbo));
	glVerify(glClearColor(powf(clearColor.x, 1.0f / 2.2f), powf(clearColor.y, 1.0f / 2.2f), powf(clearColor.z, 1.0f / 2.2f), 0.0f));
	glVerify(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void EndFrame(uint32_t width, uint32_t height)
{
	if (FruitWork::Application::renderParam->msaaFbo)
	{
		// blit the msaa buffer to the window
		glVerify(glBindFramebuffer(GL_READ_FRAMEBUFFER_EXT, FruitWork::Application::renderParam->msaaFbo));
		glVerify(glBindFramebuffer(GL_DRAW_FRAMEBUFFER_EXT, 0));
		glVerify(glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR));
	}

		// render help to back buffer
	glVerify(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	glVerify(glClear(GL_DEPTH_BUFFER_BIT));

}

void SetView(Matrix44 view, Matrix44 proj)
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(proj);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(view);
}

void SetFillMode(bool wireframe)
{
	glPolygonMode(GL_FRONT_AND_BACK, wireframe?GL_LINE:GL_FILL);
}

void SetCullMode(bool enabled)
{
	if (enabled)
		glEnable(GL_CULL_FACE);		
	else
		glDisable(GL_CULL_FACE);		
}




void GetViewRay(int x, int y, Vec3& origin, Vec3& dir)
{
	double modelview[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

	double projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	double nearPos[3];
	gluUnProject(double(x), double(y), 0.0f, modelview, projection, viewport, &nearPos[0], &nearPos[1], &nearPos[2]);

	double farPos[3];
	gluUnProject(double(x), double(y), 1.0f, modelview, projection, viewport, &farPos[0], &farPos[1], &farPos[2]);

	origin = Vec3(float(nearPos[0]), float(nearPos[1]), float(nearPos[2]));
	dir = Normalize(Vec3(float(farPos[0]-nearPos[0]), float(farPos[1]-nearPos[1]), float(farPos[2]-nearPos[2])));
}

void ReadFrame(int* backbuffer, int width, int height)
{
	glVerify(glReadBuffer(GL_BACK));
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, backbuffer);
}

void PresentFrame(bool fullsync)
{
	SDL_GL_SetSwapInterval(fullsync);
	glFinish();
	SDL_GL_SwapWindow(FruitWork::Application::renderController.GetWindow());
}

// fixes some banding artifacts with repeated blending during thickness and diffuse rendering
#define USE_HDR_DIFFUSE_BLEND 0

// vertex shader
const char *vertexPointShader = "#version 130\n" STRINGIFY(

uniform float pointRadius;  // point size in world space
uniform float pointScale;   // scale to calculate size in pixels

uniform mat4 lightTransform; 
uniform vec3 lightDir;
uniform vec3 lightDirView;

uniform vec4 colors[8];

uniform vec4 transmission;
uniform int mode;

//in int density;
in float density;
in int phase;
in vec4 velocity;

void main()
{
    // calculate window-space point size
	vec4 viewPos = gl_ModelViewMatrix*vec4(gl_Vertex.xyz, 1.0);

	gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 1.0);
	gl_PointSize = -pointScale * (pointRadius / viewPos.z);

	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = lightTransform*vec4(gl_Vertex.xyz-lightDir*pointRadius*2.0, 1.0);
	gl_TexCoord[2] = gl_ModelViewMatrix*vec4(lightDir, 0.0);

	if (mode == 1)
	{
		// density visualization
		if (density < 0.0f)
			gl_TexCoord[3].xyz = mix(vec3(0.1, 0.1, 1.0), vec3(0.1, 1.0, 1.0), -density);
		else
			gl_TexCoord[3].xyz = mix(vec3(1.0, 1.0, 1.0), vec3(0.1, 0.2, 1.0), density);
	}
	else if (mode == 2)
	{
		gl_PointSize *= clamp(gl_Vertex.w*0.25, 0.0f, 1.0);

		gl_TexCoord[3].xyzw = vec4(clamp(gl_Vertex.w*0.05, 0.0f, 1.0));
	}
	else
	{
		gl_TexCoord[3].xyz = mix(colors[phase % 8].xyz*2.0, vec3(1.0), 0.1);
	}

	gl_TexCoord[4].xyz = gl_Vertex.xyz;
	gl_TexCoord[5].xyz = viewPos.xyz;
}
);

// pixel shader for rendering points as shaded spheres
const char *fragmentPointShader = STRINGIFY(

uniform vec3 lightDir;
uniform vec3 lightPos;
uniform float spotMin;
uniform float spotMax;
uniform int mode;

uniform sampler2DShadow shadowTex;
uniform vec2 shadowTaps[12];
uniform float pointRadius;  // point size in world space

// sample shadow map
float shadowSample()
{
	vec3 pos = vec3(gl_TexCoord[1].xyz/gl_TexCoord[1].w);
	vec3 uvw = (pos.xyz*0.5)+vec3(0.5);

	// user clip
	if (uvw.x  < 0.0 || uvw.x > 1.0)
		return 1.0;
	if (uvw.y < 0.0 || uvw.y > 1.0)
		return 1.0;
	
	float s = 0.0;
	float radius = 0.002;

	for (int i=0; i < 8; i++)
	{
		s += shadow2D(shadowTex, vec3(uvw.xy + shadowTaps[i]*radius, uvw.z)).r;
	}

	s /= 8.0;
	return s;
}

float sqr(float x) { return x*x; }

void main()
{
    // calculate normal from texture coordinates
    vec3 normal;
    normal.xy = gl_TexCoord[0].xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
    float mag = dot(normal.xy, normal.xy);
    if (mag > 1.0) discard;   // kill pixels outside circle
   	normal.z = sqrt(1.0-mag);

	if (mode == 2)
	{
		float alpha  = normal.z*gl_TexCoord[3].w;
		gl_FragColor.xyz = gl_TexCoord[3].xyz*alpha;
		gl_FragColor.w = alpha;
		return;
	}

    // calculate lighting
	float shadow = shadowSample();
	
	vec3 lVec = normalize(gl_TexCoord[4].xyz-(lightPos));
	vec3 lPos = vec3(gl_TexCoord[1].xyz/gl_TexCoord[1].w);
	float attenuation = max(smoothstep(spotMax, spotMin, dot(lPos.xy, lPos.xy)), 0.05);

	vec3 diffuse = vec3(0.9, 0.9, 0.9);
	vec3 reflectance =  gl_TexCoord[3].xyz;
	
	vec3 Lo = diffuse*reflectance*max(0.0, sqr(-dot(gl_TexCoord[2].xyz, normal)*0.5 + 0.5))*max(0.2,shadow)*attenuation;

	gl_FragColor = vec4(pow(Lo, vec3(1.0/2.2)), 1.0);

	vec3 eyePos = gl_TexCoord[5].xyz + normal*pointRadius;//*2.0;
	vec4 ndcPos = gl_ProjectionMatrix * vec4(eyePos, 1.0);
	ndcPos.z /= ndcPos.w;
	gl_FragDepth = ndcPos.z*0.5 + 0.5;
}
);

// vertex shader
const char *vertexShader = "#version 130\n" STRINGIFY(

uniform mat4 lightTransform; 
uniform vec3 lightDir;
uniform float bias;
uniform vec4 clipPlane;
uniform float expand;

uniform mat4 objectTransform;

void main()
{
	vec3 n = normalize((objectTransform*vec4(gl_Normal, 0.0)).xyz);
	vec3 p = (objectTransform*vec4(gl_Vertex.xyz, 1.0)).xyz;

    // calculate window-space point size
	gl_Position = gl_ModelViewProjectionMatrix * vec4(p + expand*n, 1.0);

	gl_TexCoord[0].xyz = n;
	gl_TexCoord[1] = lightTransform*vec4(p + n*bias, 1.0);
	gl_TexCoord[2] = gl_ModelViewMatrix*vec4(lightDir, 0.0);
	gl_TexCoord[3].xyz = p;
	gl_TexCoord[4] = gl_Color;
	gl_TexCoord[5] = gl_MultiTexCoord0;
	gl_TexCoord[6] = gl_SecondaryColor;
	gl_TexCoord[7] = gl_ModelViewMatrix*vec4(gl_Vertex.xyz, 1.0);

	gl_ClipDistance[0] = dot(clipPlane,vec4(gl_Vertex.xyz, 1.0));
}
);

const char *passThroughShader = STRINGIFY(

void main()
{
	gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);

}
);

// pixel shader for rendering points as shaded spheres
const char *fragmentShader = STRINGIFY(

uniform vec3 lightDir;
uniform vec3 lightPos;
uniform float spotMin;
uniform float spotMax;
uniform vec3 color;
uniform vec4 fogColor;

uniform sampler2DShadow shadowTex;
uniform vec2 shadowTaps[12];

uniform sampler2D tex;
uniform bool sky;

uniform bool grid;
uniform bool texture;

float sqr(float x) { return x*x; }

// sample shadow map
float shadowSample()
{
	vec3 pos = vec3(gl_TexCoord[1].xyz/gl_TexCoord[1].w);
	vec3 uvw = (pos.xyz*0.5)+vec3(0.5);

	// user clip
	if (uvw.x  < 0.0 || uvw.x > 1.0)
		return 1.0;
	if (uvw.y < 0.0 || uvw.y > 1.0)
		return 1.0;
	
	float s = 0.0;
	float radius = 0.002;

	const int numTaps = 12;

	for (int i=0; i < numTaps; i++)
	{
		s += shadow2D(shadowTex, vec3(uvw.xy + shadowTaps[i]*radius, uvw.z)).r;
	}

	s /= numTaps;
	return s;
}

float filterwidth(vec2 v)
{
  vec2 fw = max(abs(dFdx(v)), abs(dFdy(v)));
  return max(fw.x, fw.y);
}

vec2 bump(vec2 x) 
{
	return (floor((x)/2) + 2.f * max(((x)/2) - floor((x)/2) - .5f, 0.f)); 
}

float checker(vec2 uv)
{
  float width = filterwidth(uv);
  vec2 p0 = uv - 0.5 * width;
  vec2 p1 = uv + 0.5 * width;
  
  vec2 i = (bump(p1) - bump(p0)) / width;
  return i.x * i.y + (1 - i.x) * (1 - i.y);
}

void main()
{
    // calculate lighting
	float shadow = max(shadowSample(), 0.5);

	vec3 lVec = normalize(gl_TexCoord[3].xyz-(lightPos));
	vec3 lPos = vec3(gl_TexCoord[1].xyz/gl_TexCoord[1].w);
	float attenuation = max(smoothstep(spotMax, spotMin, dot(lPos.xy, lPos.xy)), 0.05);
		
	vec3 n = gl_TexCoord[0].xyz;
	vec3 color = gl_TexCoord[4].xyz;

	if (!gl_FrontFacing)
	{
		color = gl_TexCoord[6].xyz;
		n *= -1.0f;
	}

	if (grid && (n.y >0.995))
	{
		color *= 1.0 - 0.25 * checker(vec2(gl_TexCoord[3].x, gl_TexCoord[3].z));
	}
	else if (grid && abs(n.z) > 0.995)
	{
		color *= 1.0 - 0.25 * checker(vec2(gl_TexCoord[3].y, gl_TexCoord[3].x));
	}

	if (texture)
	{
		color = texture2D(tex, gl_TexCoord[5].xy).xyz;
	}
	
	// direct light term
	float wrap = 0.0;
	vec3 diffuse = color*vec3(1.0, 1.0, 1.0)*max(0.0, (-dot(lightDir, n)+wrap)/(1.0+wrap)*shadow)*attenuation;
	
	// wrap ambient term aligned with light dir
	vec3 light = vec3(0.03, 0.025, 0.025)*1.5;
	vec3 dark = vec3(0.025, 0.025, 0.03);
	vec3 ambient = 4.0*color*mix(dark, light, -dot(lightDir, n)*0.5 + 0.5)*attenuation;

	vec3 fog = mix(vec3(fogColor), diffuse + ambient, exp(gl_TexCoord[7].z*fogColor.w));

	gl_FragColor = vec4(pow(fog, vec3(1.0/2.2)), 1.0);				
}
);

void ShadowApply(GLint sprogram, Vec3 lightPos, Vec3 lightTarget, Matrix44 lightTransform, GLuint shadowTex)
{
	GLint uLightTransform = glGetUniformLocation(sprogram, "lightTransform");
	glUniformMatrix4fv(uLightTransform, 1, false, lightTransform);

	GLint uLightPos = glGetUniformLocation(sprogram, "lightPos");
	glUniform3fv(uLightPos, 1, lightPos);
	
	GLint uLightDir = glGetUniformLocation(sprogram, "lightDir");
	glUniform3fv(uLightDir, 1, Normalize(lightTarget-lightPos));

	GLint uBias = glGetUniformLocation(sprogram, "bias");
	glUniform1f(uBias, gShadowBias);

	const Vec2 taps[] = 
	{ 
		Vec2(-0.326212f,-0.40581f),Vec2(-0.840144f,-0.07358f),
		Vec2(-0.695914f,0.457137f),Vec2(-0.203345f,0.620716f),
		Vec2(0.96234f,-0.194983f),Vec2(0.473434f,-0.480026f),
		Vec2(0.519456f,0.767022f),Vec2(0.185461f,-0.893124f),
		Vec2(0.507431f,0.064425f),Vec2(0.89642f,0.412458f),
		Vec2(-0.32194f,-0.932615f),Vec2(-0.791559f,-0.59771f) 
	};
	
	GLint uShadowTaps = glGetUniformLocation(sprogram, "shadowTaps");
	glUniform2fv(uShadowTaps, 12, &taps[0].x);
	
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

}

void DrawPoints(GLuint positions, GLuint colors, GLuint indices, int n, int offset, float radius, float screenWidth, float screenAspect, float fov, Vec3 lightPos, Vec3 lightTarget, Matrix44 lightTransform, ShadowMap* shadowMap, bool showDensity)
{
	static int sprogram = -1;
	if (sprogram == -1)
	{
		sprogram = CompileProgram(vertexPointShader, fragmentPointShader);
	}

	if (sprogram)
	{
		glEnable(GL_POINT_SPRITE);
		glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		//glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
	
		int mode = 0;
		if (showDensity)
			mode = 1;
		if (shadowMap == NULL)
			mode = 2;

		glVerify(glUseProgram(sprogram));
		glVerify(glUniform1f( glGetUniformLocation(sprogram, "pointRadius"), radius));
		glVerify(glUniform1f( glGetUniformLocation(sprogram, "pointScale"), screenWidth/screenAspect * (1.0f / (tanf(fov*0.5f)))));
		glVerify(glUniform1f( glGetUniformLocation(sprogram, "spotMin"), gSpotMin));
		glVerify(glUniform1f( glGetUniformLocation(sprogram, "spotMax"), gSpotMax));
		glVerify(glUniform1i( glGetUniformLocation(sprogram, "mode"), mode));
		glVerify(glUniform4fv( glGetUniformLocation(sprogram, "colors"), 8, (float*)&gColors[0].r));

		// set shadow parameters
		ShadowApply(sprogram, lightPos, lightTarget, lightTransform, shadowMap->texture);

		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, positions);
		glVertexPointer(4, GL_FLOAT, 0, 0);

		int d = glGetAttribLocation(sprogram, "density");
		int p = glGetAttribLocation(sprogram, "phase");

		if (d != -1)
		{
			glVerify(glEnableVertexAttribArray(d));
			glVerify(glBindBuffer(GL_ARRAY_BUFFER, colors));
			glVerify(glVertexAttribPointer(d, 1,  GL_FLOAT, GL_FALSE, 0, 0));	// densities
		}

		if (p != -1)
		{
			glVerify(glEnableVertexAttribArray(p));
			glVerify(glBindBuffer(GL_ARRAY_BUFFER, colors));
			glVerify(glVertexAttribIPointer(p, 1,  GL_INT, 0, 0));			// phases
		}

		glVerify(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices));

		glVerify(glDrawElements(GL_POINTS, n, GL_UNSIGNED_INT, (const void*)(offset*sizeof(int))));

		glVerify(glUseProgram(0));
		glVerify(glBindBuffer(GL_ARRAY_BUFFER, 0));
		glVerify(glDisableClientState(GL_VERTEX_ARRAY));	
		
		if (d != -1)
			glVerify(glDisableVertexAttribArray(d));
		if (p != -1)
			glVerify(glDisableVertexAttribArray(p));
		
		glDisable(GL_POINT_SPRITE);
		glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);		
	}
}
void DrawPlane(const Vec4& p);

static GLuint s_diffuseProgram = GLuint(-1);
static GLuint s_shadowProgram = GLuint(-1);

static const int kShadowResolution = 2048;

void ShadowDestroy(ShadowMap* map)
{
	glVerify(glDeleteTextures(1, &map->texture));
	glVerify(glDeleteFramebuffers(1, &map->framebuffer));

	delete map;
}

void ShadowBegin(ShadowMap* map)
{
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(8.f, 8.f);

	glVerify(glBindFramebuffer(GL_FRAMEBUFFER, map->framebuffer));

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, kShadowResolution, kShadowResolution);

	// draw back faces (for teapot)
	glDisable(GL_CULL_FACE);

	// bind shadow shader
	if (s_shadowProgram == GLuint(-1))
		s_shadowProgram = CompileProgram(vertexShader, passThroughShader);

	glUseProgram(s_shadowProgram);
	glVerify(glUniformMatrix4fv(glGetUniformLocation(s_shadowProgram, "objectTransform"), 1, false, Matrix44::kIdentity));
}

void ShadowEnd()
{
	glDisable(GL_POLYGON_OFFSET_FILL);

	glVerify(glBindFramebuffer(GL_FRAMEBUFFER, FruitWork::Application::renderParam->msaaFbo));

	glEnable(GL_CULL_FACE);
	glUseProgram(0);
}

void BindSolidShader(uint32_t width, uint32_t height, Vec3 lightPos, Vec3 lightTarget, Matrix44 lightTransform, ShadowMap* shadowMap, float bias, Vec4 fogColor)
{
	glVerify(glViewport(0, 0, width, height));

	if (s_diffuseProgram == GLuint(-1))
		s_diffuseProgram = CompileProgram(vertexShader, fragmentShader);

	if (s_diffuseProgram)
	{
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);		

		glVerify(glUseProgram(s_diffuseProgram));
		glVerify(glUniform1i(glGetUniformLocation(s_diffuseProgram, "grid"), 0));
		glVerify(glUniform1f( glGetUniformLocation(s_diffuseProgram, "spotMin"), gSpotMin));
		glVerify(glUniform1f( glGetUniformLocation(s_diffuseProgram, "spotMax"), gSpotMax));
		glVerify(glUniform4fv( glGetUniformLocation(s_diffuseProgram, "fogColor"), 1, fogColor));

		glVerify(glUniformMatrix4fv( glGetUniformLocation(s_diffuseProgram, "objectTransform"), 1, false, Matrix44::kIdentity));

		// set shadow parameters
		ShadowApply(s_diffuseProgram, lightPos, lightTarget, lightTransform, shadowMap->texture);
	}
}

void UnbindSolidShader()
{
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	glUseProgram(0);
}

void DrawPlanes(Vec4* planes, int n, float bias)
{
	// diffuse 		
	glColor3f(0.9f, 0.9f, 0.9f);

	GLint uBias = glGetUniformLocation(s_diffuseProgram, "bias");
	glVerify(glUniform1f(uBias, 0.0f));
	GLint uGrid = glGetUniformLocation(s_diffuseProgram, "grid");
	glVerify(glUniform1i(uGrid, 1));
	GLint uExpand = glGetUniformLocation(s_diffuseProgram, "expand");
	glVerify(glUniform1f(uExpand, 0.0f));

	for (int i=0; i < n; ++i)
	{
		Vec4 p = planes[i];
		p.w -= bias;

		DrawPlane(p, false);
	}

	glVerify(glUniform1i(uGrid, 0));
	glVerify(glUniform1f(uBias, gShadowBias));
}

void DrawMesh(const Mesh* m, Vec3 color)
{
	if (m)
	{
		glVerify(glColor3fv(color));
		glVerify(glSecondaryColor3fv(color));

		glVerify(glBindBuffer(GL_ARRAY_BUFFER, 0));
		glVerify(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

		glVerify(glEnableClientState(GL_NORMAL_ARRAY));
		glVerify(glEnableClientState(GL_VERTEX_ARRAY));

		glVerify(glNormalPointer(GL_FLOAT, sizeof(float) * 3, &m->m_normals[0]));
		glVerify(glVertexPointer(3, GL_FLOAT, sizeof(float) * 3, &m->m_positions[0]));

		if (m->m_colours.size())
		{
			glVerify(glEnableClientState(GL_COLOR_ARRAY));
			glVerify(glColorPointer(4, GL_FLOAT, 0, &m->m_colours[0]));
		}

		glVerify(glDrawElements(GL_TRIANGLES, m->GetNumFaces() * 3, GL_UNSIGNED_INT, &m->m_indices[0]));

		glVerify(glDisableClientState(GL_VERTEX_ARRAY));
		glVerify(glDisableClientState(GL_NORMAL_ARRAY));

		if (m->m_colours.size())
			glVerify(glDisableClientState(GL_COLOR_ARRAY));
	}
}

void DrawCloth(const Vec4* positions, const Vec4* normals, const float* uvs, const int* indices, int numTris, int numPositions, int colorIndex, float expand, bool twosided, bool smooth)
{ 
	if (!numTris)
		return;

	if (twosided)
		glDisable(GL_CULL_FACE);

#if 1
	GLint program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);

	if (program == GLint(s_diffuseProgram))
	{
		GLint uBias = glGetUniformLocation(s_diffuseProgram, "bias");
		glUniform1f(uBias, 0.0f);

		GLint uExpand = glGetUniformLocation(s_diffuseProgram, "expand");
		glUniform1f(uExpand, expand);
	}
#endif

	glColor3fv(Colour(1.0, 1.0, 1.0));
	glSecondaryColor3fv(Colour(1.0, 1.0, 1.0));

	glVerify(glBindBuffer(GL_ARRAY_BUFFER, 0));
	glVerify(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

	glVerify(glEnableClientState(GL_VERTEX_ARRAY));
	glVerify(glEnableClientState(GL_NORMAL_ARRAY));

	glVerify(glVertexPointer(3, GL_FLOAT, sizeof(float)*4, positions));
	glVerify(glNormalPointer(GL_FLOAT, sizeof(float)*4, normals));
	
	glVerify(glDrawElements(GL_TRIANGLES, numTris*3, GL_UNSIGNED_INT, indices));

	glVerify(glDisableClientState(GL_VERTEX_ARRAY));
	glVerify(glDisableClientState(GL_NORMAL_ARRAY));

	if (twosided)
		glEnable(GL_CULL_FACE);

#if 1
	if (program == GLint(s_diffuseProgram))
	{
		GLint uBias = glGetUniformLocation(s_diffuseProgram, "bias");
		glUniform1f(uBias, gShadowBias);

		GLint uExpand = glGetUniformLocation(s_diffuseProgram, "expand");
		glUniform1f(uExpand, 0.0f);
	}
#endif

}

void DrawRope(Vec4* positions, int* indices, int numIndices, float radius, int color)
{
	if (numIndices < 2)
		return;

	std::vector<Vec3> vertices;
	std::vector<Vec3> normals;
	std::vector<int> triangles;
	
	// flatten curve
	std::vector<Vec3> curve(numIndices);
	for (int i=0; i < numIndices; ++i)
		curve[i] = Vec3(positions[indices[i]]);

	const int resolution = 8;
	const int smoothing = 3;

	vertices.reserve(resolution*numIndices*smoothing);
	normals.reserve(resolution*numIndices*smoothing);
	triangles.reserve(numIndices*resolution*6*smoothing);

	Extrude(&curve[0], int(curve.size()), vertices, normals, triangles, radius, resolution, smoothing);
		
	glVerify(glDisable(GL_CULL_FACE));
	glVerify(glColor3fv(gColors[color%8]*1.5f));
	glVerify(glSecondaryColor3fv(gColors[color%8]*1.5f));

	glVerify(glBindBuffer(GL_ARRAY_BUFFER, 0));

	glVerify(glEnableClientState(GL_VERTEX_ARRAY));
	glVerify(glEnableClientState(GL_NORMAL_ARRAY));

	glVerify(glVertexPointer(3, GL_FLOAT, sizeof(float)*3, &vertices[0]));
	glVerify(glNormalPointer(GL_FLOAT, sizeof(float)*3, &normals[0]));

	glVerify(glDrawElements(GL_TRIANGLES, GLsizei(triangles.size()), GL_UNSIGNED_INT, &triangles[0]));

	glVerify(glDisableClientState(GL_VERTEX_ARRAY));
	glVerify(glDisableClientState(GL_NORMAL_ARRAY));
	glVerify(glEnable(GL_CULL_FACE));

}


struct ReflectMap
{
	GLuint texture;

	int width;
	int height;
};

ReflectMap* ReflectCreate(int width, int height)
{
	GLuint texture;

	// copy frame buffer to texture
	glVerify(glActiveTexture(GL_TEXTURE0));
	glVerify(glEnable(GL_TEXTURE_2D));

	glVerify(glGenTextures(1, &texture));
	glVerify(glBindTexture(GL_TEXTURE_2D, texture));

	glVerify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)); 
	glVerify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)); 	 
    glVerify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    glVerify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		
	glVerify(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));

	ReflectMap* map = new ReflectMap();
	map->texture = texture;
	map->width = width;
	map->height = height;

	return map;
}

void ReflectDestroy(ReflectMap* map)
{
	glVerify(glDeleteTextures(1, &map->texture));

	delete map;
}

void ReflectBegin(ReflectMap* map, Vec4 plane, int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Matrix44 scale = Matrix44::kIdentity;
	scale.columns[0][0] *= -2.0f;
	scale.columns[1][1] *= -2.0f;
	scale.columns[2][2] *= -2.0f;
	scale.columns[3][3] *= -2.0f;

	Matrix44 reflect = (scale*Outer(Vec4(plane.x, plane.y, plane.z, 0.0f), plane));
	reflect.columns[0][0] += 1.0f;
	reflect.columns[1][1] += 1.0f;
	reflect.columns[2][2] += 1.0f;
	reflect.columns[3][3] += 1.0f;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(reflect);

	glVerify(glFrontFace(GL_CW));
	glVerify(glEnable(GL_CLIP_PLANE0));

	glVerify(glUniform4fv( glGetUniformLocation(s_diffuseProgram, "clipPlane"), 1, plane));
}

void ReflectEnd(ReflectMap* map, int width, int height)
{
	// copy frame buffer to texture
	glVerify(glActiveTexture(GL_TEXTURE0));
	glVerify(glEnable(GL_TEXTURE_2D));
	glVerify(glBindTexture(GL_TEXTURE_2D, map->texture));

	glVerify(glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height));
	
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glVerify(glDisable(GL_CLIP_PLANE0));
	glVerify(glFrontFace(GL_CCW));

	glBindFramebuffer(GL_FRAMEBUFFER, FruitWork::Application::renderParam->msaaFbo);

	// maybe this wrong (get width and height from render controller)
	glViewport(0, 0, width, height);
}




// pixel shader for rendering points as shaded spheres
const char *fragmentPointDepthShader = STRINGIFY(

uniform float pointRadius;  // point size in world space

void main()
{
    // calculate normal from texture coordinates
    vec3 normal;
    normal.xy = gl_TexCoord[0].xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
    float mag = dot(normal.xy, normal.xy);
    if (mag > 1.0) discard;   // kill pixels outside circle
   	normal.z = sqrt(1.0-mag);

	vec3 eyePos = gl_TexCoord[1].xyz + normal*pointRadius*2.0;
	vec4 ndcPos = gl_ProjectionMatrix * vec4(eyePos, 1.0);
	ndcPos.z /= ndcPos.w;

	gl_FragColor = vec4(eyePos.z, 1.0, 1.0, 1.0);
	gl_FragDepth = ndcPos.z*0.5 + 0.5;
}
);

void RenderFullscreenQuad()
{
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-1.0f, -1.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(1.0f, -1.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(1.0f, 1.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);

	glEnd();
}

void RenderEllipsoids(FluidRenderer* render, FluidRenderBuffers buffers, int n, int offset, float radius, float screenWidth, float screenAspect, float fov, Vec3 lightPos, Vec3 lightTarget, Matrix44 lightTransform, ShadowMap* shadowMap, Vec4 color, float blur, float ior, bool debug)
{
#if !ENABLE_SIMPLE_FLUID
	// resolve msaa back buffer to texture
	glVerify(glBindFramebuffer(GL_READ_FRAMEBUFFER_EXT, FruitWork::Application::renderParam->msaaFbo));
	glVerify(glBindFramebuffer(GL_DRAW_FRAMEBUFFER_EXT, render->mSceneFbo));
	glVerify(glBlitFramebuffer(0, 0, GLsizei(screenWidth), GLsizei(screenWidth/screenAspect), 0, 0, GLsizei(screenWidth), GLsizei(screenWidth/screenAspect), GL_COLOR_BUFFER_BIT, GL_LINEAR));

	//thickness texture
	glVerify(glBindFramebuffer(GL_FRAMEBUFFER, render->mThicknessFbo));
	glVerify(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render->mThicknessTex, 0));
	glVerify(glDrawBuffer(GL_COLOR_ATTACHMENT0));

	glViewport(0, 0, GLsizei(screenWidth), GLsizei(screenWidth/screenAspect));
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	glDepthMask(GL_TRUE);
	glDisable(GL_CULL_FACE);

	if (FruitWork::Application::renderBuffers->mesh)
		DrawMesh(FruitWork::Application::renderBuffers->mesh, Vec3(1.0f));

	FruitWork::Application::renderController.DrawShapes();

	glClear(GL_COLOR_BUFFER_BIT);

	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glDepthMask(GL_FALSE);

	// make sprites larger to get smoother thickness texture
	const float thicknessScale = 4.0f; 

	glUseProgram(render->mPointThicknessProgram);
	glUniform1f( glGetUniformLocation(render->mPointThicknessProgram, "pointRadius"), thicknessScale*radius);
	glUniform1f( glGetUniformLocation(render->mPointThicknessProgram, "pointScale"), screenWidth/screenAspect * (1.0f / (tanf(fov*0.5f))));

	glEnableClientState(GL_VERTEX_ARRAY);			
	glBindBuffer(GL_ARRAY_BUFFER, buffers.mPositionVBO);
	glVertexPointer(3, GL_FLOAT, sizeof(float)*4, (void*)(offset*sizeof(float)*4));

	glDrawArrays(GL_POINTS, 0, n);

	glUseProgram(0);
	glDisableClientState(GL_VERTEX_ARRAY);	
	glDisable(GL_POINT_SPRITE);
	glDisable(GL_BLEND);
#endif

	// depth texture
	glVerify(glBindFramebuffer(GL_FRAMEBUFFER, render->mDepthFbo));
	glVerify(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_ARB, render->mDepthTex, 0));
	glVerify(glDrawBuffer(GL_COLOR_ATTACHMENT0));

	// draw points
	//glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
	glDisable(GL_POINT_SPRITE);
	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glViewport(0, 0, int(screenWidth), int(screenWidth/screenAspect));
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const float viewHeight = tanf(fov/2.0f);

	glUseProgram(render->mEllipsoidDepthProgram);
	glUniform3fv( glGetUniformLocation(render->mEllipsoidDepthProgram, "invViewport"), 1, Vec3(1.0f/screenWidth, screenAspect/screenWidth, 1.0f));
	glUniform3fv( glGetUniformLocation(render->mEllipsoidDepthProgram, "invProjection"), 1, Vec3(screenAspect*viewHeight, viewHeight, 1.0f));

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, buffers.mPositionVBO);
	glVertexPointer(3, GL_FLOAT, sizeof(float)*4, 0);//(void*)(offset*sizeof(float)*4));

	// ellipsoid eigenvectors
	int s1 = glGetAttribLocation(render->mEllipsoidDepthProgram, "q1");
	glEnableVertexAttribArray(s1);
	glBindBuffer(GL_ARRAY_BUFFER, buffers.mAnisotropyVBO[0]);
	glVertexAttribPointer(s1, 4, GL_FLOAT, GL_FALSE, 0, 0);// (void*)(offset*sizeof(float)*4));

	int s2 = glGetAttribLocation(render->mEllipsoidDepthProgram, "q2");
	glEnableVertexAttribArray(s2);
	glBindBuffer(GL_ARRAY_BUFFER, buffers.mAnisotropyVBO[1]);
	glVertexAttribPointer(s2, 4, GL_FLOAT, GL_FALSE, 0, 0);//(void*)(offset*sizeof(float)*4));

	int s3 = glGetAttribLocation(render->mEllipsoidDepthProgram, "q3");
	glEnableVertexAttribArray(s3);
	glBindBuffer(GL_ARRAY_BUFFER, buffers.mAnisotropyVBO[2]);
	glVertexAttribPointer(s3, 4, GL_FLOAT, GL_FALSE, 0, 0);// (void*)(offset*sizeof(float)*4));
	
	glVerify(glDrawArrays(GL_POINTS, offset, n));

	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableVertexAttribArray(s1);
	glDisableVertexAttribArray(s2);
	glDisableVertexAttribArray(s3);

	glDisable(GL_POINT_SPRITE);

	//---------------------------------------------------------------
	// blur

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	glVerify(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render->mDepthSmoothTex, 0));
	glUseProgram(render->mDepthBlurProgram);
	
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);	
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, render->mDepthTex);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, render->mThicknessTex);

	glVerify(glUniform1f( glGetUniformLocation(render->mDepthBlurProgram, "blurRadiusWorld"), radius*0.5f));	// blur half the radius by default
	glVerify(glUniform1f( glGetUniformLocation(render->mDepthBlurProgram, "blurScale"), screenWidth/screenAspect * (1.0f / (tanf(fov*0.5f)))));
	glVerify(glUniform2fv( glGetUniformLocation(render->mDepthBlurProgram, "invTexScale"), 1, Vec2(1.0f/screenAspect, 1.0f)));
	glVerify(glUniform1f( glGetUniformLocation(render->mDepthBlurProgram, "blurFalloff"),  blur));
	glVerify(glUniform1i( glGetUniformLocation(render->mDepthBlurProgram, "depthTex"), 0));
	glVerify(glUniform1i( glGetUniformLocation(render->mDepthBlurProgram, "thicknessTex"), 1));
	glVerify(glUniform1i(glGetUniformLocation(render->mDepthBlurProgram, "debug"), debug));

	glVerify(RenderFullscreenQuad());

	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);	

	//---------------------------------------------------------------
	// composite with scene

	glVerify(glBindFramebuffer(GL_FRAMEBUFFER, FruitWork::Application::renderParam->msaaFbo));
	glVerify(glEnable(GL_DEPTH_TEST));
	glVerify(glDepthMask(GL_TRUE));
	glVerify(glDisable(GL_BLEND));
	glVerify(glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA));

	glVerify(glUseProgram(render->mCompositeProgram));	

	glVerify(glUniform2fv(glGetUniformLocation(render->mCompositeProgram, "invTexScale"), 1, Vec2(1.0f/screenWidth, screenAspect/screenWidth)));
	glVerify(glUniform2fv(glGetUniformLocation(render->mCompositeProgram, "clipPosToEye"), 1, Vec2(tanf(fov*0.5f)*screenAspect, tanf(fov*0.5f))));
	glVerify(glUniform4fv(glGetUniformLocation(render->mCompositeProgram, "color"), 1, color));
	glVerify(glUniform1f(glGetUniformLocation(render->mCompositeProgram, "ior"),  ior));
	glVerify(glUniform1f(glGetUniformLocation(render->mCompositeProgram, "spotMin"), gSpotMin));
	glVerify(glUniform1f(glGetUniformLocation(render->mCompositeProgram, "spotMax"), gSpotMax));
	glVerify(glUniform1i(glGetUniformLocation(render->mCompositeProgram, "debug"), debug));

	glVerify(glUniform3fv(glGetUniformLocation(render->mCompositeProgram, "lightPos"), 1, lightPos));
	glVerify(glUniform3fv(glGetUniformLocation(render->mCompositeProgram, "lightDir"), 1, -Normalize(lightTarget-lightPos)));
	glVerify(glUniformMatrix4fv(glGetUniformLocation(render->mCompositeProgram, "lightTransform"), 1, false, lightTransform));
	
	const Vec2 taps[] = 
	{ 
		Vec2(-0.326212f,-0.40581f),Vec2(-0.840144f,-0.07358f),
		Vec2(-0.695914f,0.457137f),Vec2(-0.203345f,0.620716f),
		Vec2(0.96234f,-0.194983f),Vec2(0.473434f,-0.480026f),
		Vec2(0.519456f,0.767022f),Vec2(0.185461f,-0.893124f),
		Vec2(0.507431f,0.064425f),Vec2(0.89642f,0.412458f),
		Vec2(-0.32194f,-0.932615f),Vec2(-0.791559f,-0.59771f) 
	};
	
	glVerify(glUniform2fv(glGetUniformLocation(render->mCompositeProgram, "shadowTaps"), 12, &taps[0].x));

	// smoothed depth tex
	glVerify(glActiveTexture(GL_TEXTURE0));
	glVerify(glEnable(GL_TEXTURE_2D));
	glVerify(glBindTexture(GL_TEXTURE_2D, render->mDepthSmoothTex));

	// shadow tex
	glVerify(glActiveTexture(GL_TEXTURE1));
	glVerify(glEnable(GL_TEXTURE_2D));
	glVerify(glBindTexture(GL_TEXTURE_2D, shadowMap->texture));

	// thickness tex
	glVerify(glActiveTexture(GL_TEXTURE2));
	glVerify(glEnable(GL_TEXTURE_2D));
	glVerify(glBindTexture(GL_TEXTURE_2D, render->mThicknessTex));

	// scene tex
	glVerify(glActiveTexture(GL_TEXTURE3));
	glVerify(glEnable(GL_TEXTURE_2D));
	glVerify(glBindTexture(GL_TEXTURE_2D, render->mSceneTex));

	/*
	// reflection tex
	glVerify(glActiveTexture(GL_TEXTURE5));
	glVerify(glEnable(GL_TEXTURE_2D));
	glVerify(glBindTexture(GL_TEXTURE_2D, reflectMap->texture));	
	*/

	glVerify(glUniform1i(glGetUniformLocation(render->mCompositeProgram, "tex"), 0));
	glVerify(glUniform1i(glGetUniformLocation(render->mCompositeProgram, "shadowTex"), 1));
	glVerify(glUniform1i(glGetUniformLocation(render->mCompositeProgram, "thicknessTex"), 2));
	glVerify(glUniform1i(glGetUniformLocation(render->mCompositeProgram, "sceneTex"), 3));
	glVerify(glUniform1i(glGetUniformLocation(render->mCompositeProgram, "reflectTex"), 5));

	// -- end shadowing
	
	// ignores projection matrices
	glVerify(RenderFullscreenQuad());

	// reset state
	glActiveTexture(GL_TEXTURE5);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE3);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

struct GpuMesh
{
	GLuint mPositionsVBO;
	GLuint mNormalsVBO;
	GLuint mIndicesIBO;

	int mNumVertices;
	int mNumFaces;
};

GpuMesh* CreateGpuMesh(const Mesh* m)
{
	GpuMesh* mesh = new GpuMesh();

	mesh->mNumVertices = m->GetNumVertices();
	mesh->mNumFaces = m->GetNumFaces();

	// vbos
	glVerify(glGenBuffers(1, &mesh->mPositionsVBO));
	glVerify(glBindBuffer(GL_ARRAY_BUFFER, mesh->mPositionsVBO));
	glVerify(glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*m->m_positions.size(), &m->m_positions[0], GL_STATIC_DRAW));

	glVerify(glGenBuffers(1, &mesh->mNormalsVBO));
	glVerify(glBindBuffer(GL_ARRAY_BUFFER, mesh->mNormalsVBO));
	glVerify(glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*m->m_normals.size(), &m->m_normals[0], GL_STATIC_DRAW));

	glVerify(glGenBuffers(1, &mesh->mIndicesIBO));
	glVerify(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->mIndicesIBO));
	glVerify(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*m->m_indices.size(), &m->m_indices[0], GL_STATIC_DRAW));
	glVerify(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

	return mesh;
}

void DestroyGpuMesh(GpuMesh* m)
{
	glVerify(glDeleteBuffers(1, &m->mPositionsVBO));
	glVerify(glDeleteBuffers(1, &m->mNormalsVBO));
	glVerify(glDeleteBuffers(1, &m->mIndicesIBO));
}

void DrawGpuMesh(GpuMesh* m, const Matrix44& xform, const Vec3& color)
{
	if (m)
	{
		GLint program;
		glGetIntegerv(GL_CURRENT_PROGRAM, &program);

		if (program)
			glUniformMatrix4fv( glGetUniformLocation(program, "objectTransform"), 1, false, xform);

		glVerify(glColor3fv(color));
		glVerify(glSecondaryColor3fv(color));

		glVerify(glEnableClientState(GL_VERTEX_ARRAY));
		glVerify(glBindBuffer(GL_ARRAY_BUFFER, m->mPositionsVBO));
		glVerify(glVertexPointer(3, GL_FLOAT, sizeof(float)*3, 0));	

		glVerify(glEnableClientState(GL_NORMAL_ARRAY));
		glVerify(glBindBuffer(GL_ARRAY_BUFFER, m->mNormalsVBO));
		glVerify(glNormalPointer(GL_FLOAT, sizeof(float)*3, 0));
		
		glVerify(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->mIndicesIBO));

		glVerify(glDrawElements(GL_TRIANGLES, m->mNumFaces*3, GL_UNSIGNED_INT, 0));

		glVerify(glDisableClientState(GL_VERTEX_ARRAY));
		glVerify(glDisableClientState(GL_NORMAL_ARRAY));

		glVerify(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		glVerify(glBindBuffer(GL_ARRAY_BUFFER, 0));	

		if (program)
			glUniformMatrix4fv(glGetUniformLocation(program, "objectTransform"), 1, false, Matrix44::kIdentity);
	}
}

void DrawGpuMeshInstances(GpuMesh* m, const Matrix44* xforms, int n, const Vec3& color)
{
	if (m)
	{
		GLint program;
		glGetIntegerv(GL_CURRENT_PROGRAM, &program);

		GLint param = glGetUniformLocation(program, "objectTransform");

		glVerify(glColor3fv(color));
		glVerify(glSecondaryColor3fv(color));

		glVerify(glEnableClientState(GL_VERTEX_ARRAY));
		glVerify(glBindBuffer(GL_ARRAY_BUFFER, m->mPositionsVBO));
		glVerify(glVertexPointer(3, GL_FLOAT, sizeof(float)*3, 0));	

		glVerify(glEnableClientState(GL_NORMAL_ARRAY));
		glVerify(glBindBuffer(GL_ARRAY_BUFFER, m->mNormalsVBO));
		glVerify(glNormalPointer(GL_FLOAT, sizeof(float)*3, 0));
		
		glVerify(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->mIndicesIBO));

		for (int i=0; i < n; ++i)
		{
			if (program)
				glUniformMatrix4fv( param, 1, false, xforms[i]);

			glVerify(glDrawElements(GL_TRIANGLES, m->mNumFaces*3, GL_UNSIGNED_INT, 0));
		}

		glVerify(glDisableClientState(GL_VERTEX_ARRAY));
		glVerify(glDisableClientState(GL_NORMAL_ARRAY));

		glVerify(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}
}

void BeginLines()
{
	glUseProgram(0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glLineWidth(1.0f);

	for (int i = 0; i < 8; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glDisable(GL_TEXTURE_2D);
	}

	glBegin(GL_LINES);
}

void DrawLine(const Vec3& p, const Vec3& q, const Vec4& color)
{
	glColor4fv(color);
	glVertex3fv(p);
	glVertex3fv(q);
}

void EndLines()
{
	glEnd();
}

void BeginPoints(float size)
{
	glPointSize(size);

	glUseProgram(0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	for (int i = 0; i < 8; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glDisable(GL_TEXTURE_2D);
	}

	glBegin(GL_POINTS);
}

void DrawPoint(const Vec3& p, const Vec4& color)
{
	glColor3fv(color);
	glVertex3fv(p);
}

void EndPoints()
{
	glEnd();
}

