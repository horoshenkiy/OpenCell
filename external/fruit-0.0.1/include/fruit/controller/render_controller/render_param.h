#ifndef RENDER_PARAM_H
#define RENDER_PARAM_H

#include "render_gl/shader.h"

namespace FruitWork {
namespace Render {

struct RenderParam {

	static RenderParam& Instance() {
		static RenderParam renderParam;
		return renderParam;
	}

	static RenderParam& Get() {
		return Instance();
	}

	/////////////////////////////////////////////////////////////
	// MSAA
	//////////////////////////////////////////////////////////
	int msaaSamples = 8;
	GLuint msaaFbo = 0;
	GLuint msaaColorBuf = 0;
	GLuint msaaDepthBuf = 0;

	//////////////////////////////////////////////////////////
	// param of type render
	/////////////////////////////////////////////////////////

	// draw GUI
	bool showGUI = true;
	
	bool drawEllipsoids = true;
	bool drawOpaque = false;
	bool drawPoints = false;
	bool drawCloth = true;
	bool drawBases = false;
	bool drawContacts = false;
	bool drawNormals = false;
	bool drawShapeGrid = false;
	bool drawDensity = false;
	int  drawSprings = 0;		// 0: no draw, 1: draw stretch 2: draw tether

	// don't work, sorry 
	bool drawMesh = true;

	// move planes along their normal for rendering
	float drawPlaneBias = 0.0f;

	/////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

	// params of scale
	float pointScale = 1.0f;
	float g_ropeScale = 1.0f;

	float blur = 1.0f;
	float ior = 1.0f;

	float diffuseScale = 0.5f;
	float diffuseMotionScale = 1.0f;
	bool diffuseShadow = false;
	float diffuseInscatter = 0.8f;
	float diffuseOutscatter = 0.53f;

	// colors
	Vec4 fluidColor = Vec4(0.1f, 0.4f, 0.8f, 1.0f);
	Vec4 diffuseColor;
	Vec3 meshColor = Vec3(0.9f, 0.9f, 0.9f);
	Vec3 clearColor;

	float fogDistance = 0.005f;

	bool wireframe = false;

	//for cloth?
	float expandCloth = 0.0f;

	// synchronize compute and render with realtime
	bool vsync = false;

private:

	RenderParam() = default;

	RenderParam(const RenderParam &other) = delete;
	RenderParam operator=(const RenderParam &other) = delete;
};

}
}

#endif // RENDER_PARAM_H