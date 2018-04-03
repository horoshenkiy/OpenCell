// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
#pragma once

#define STRINGIFY(A) #A

#include <flex/core/maths.h>
#include <flex/core/mesh.h>

namespace FruitWork {
namespace Render {

namespace Fluid { struct FluidRenderer; }

namespace GL {

typedef unsigned int VertexBuffer;
typedef unsigned int IndexBuffer;
typedef unsigned int Texture;

//void InitRender(SDL_Window* window, bool fullscreen, int msaaSamples);
void DestroyRender();
void ReshapeRender(SDL_Window* window);

void StartFrame(Vec4 clearColor, GLuint msaaFBO);
void PresentFrame(bool fullsync, SDL_Window *window);
void ReadFrame(int* backbuffer, int width, int height); // read back pixel values
void EndFrame(uint32_t width, uint32_t height, GLuint msaaFBO);

void GetViewRay(int x, int y, Vec3& origin, Vec3& dir);

void SetView(Matrix44 view, Matrix44 proj);
void SetFillMode(bool wireframe);
void SetCullMode(bool enabled);

// debug draw methods
void BeginLines();
void DrawLine(const Vec3& p, const Vec3& q, const Vec4& color);
void EndLines();

// shadowing
struct ShadowMap;
ShadowMap* ShadowCreate();
void ShadowDestroy(ShadowMap* map);
void ShadowBegin(ShadowMap* map);
void ShadowEnd(GLuint msaaFBO);

// primitive draw methods
void DrawPlanes(Vec4* planes, int n, float bias);
void DrawPoints(VertexBuffer positions, VertexBuffer color, IndexBuffer indices, int n, int offset, float radius, float screenWidth, float screenAspect, float fov, Vec3 lightPos, Vec3 lightTarget, Matrix44 lightTransform, ShadowMap* shadowTex, bool showDensity);
void DrawMesh(const Mesh*, Vec3 color);
void DrawCloth(const Vec4* positions, const Vec4* normals, const float* uvs, const int* indices, int numTris, int numPositions, int colorIndex=3, float expand=0.0f, bool twosided=true, bool smooth=true);
void DrawBuffer(float* buffer, Vec3 camPos, Vec3 lightPos);
void DrawRope(Vec4* positions, int* indices, int numIndices, float radius, int color);
void DrawShapes();

struct GpuMesh;

GpuMesh* CreateGpuMesh(const Mesh* m);
void DestroyGpuMesh(GpuMesh* m);
void DrawGpuMesh(GpuMesh* m, const Matrix44& xform, const Vec3& color);
void DrawGpuMeshInstances(GpuMesh* m, const Matrix44* xforms, int n, const Vec3& color);

// main lighting shader
void BindSolidShader(uint32_t width, uint32_t height, Vec3 lightPos, Vec3 lightTarget, Matrix44 lightTransform, ShadowMap* shadowTex, float bias, Vec4 fogColor);
void UnbindSolidShader();

// screen space fluid rendering
void RenderEllipsoids(
					Fluid::FluidRenderer* render, 
					Fluid::FluidRenderBuffers buffers, 
					Mesh *mesh,
					int n, int offset, float radius, 
					float screenWidth, float screenAspect, GLuint msaaFBO,
					float fov, 
					Vec3 lightPos, Vec3 lightTarget, Matrix44 lightTransform, 
					ShadowMap* shadowTex, 
					Vec4 color, float blur, float ior, 
					bool debug);

}
}
}
