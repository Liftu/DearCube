#pragma once
#pragma comment(lib, "Opengl32.lib")

#include <Windows.h>
#include <gl/GL.h>
#include <cmath>

#define PI 3.14159265358979323846f

namespace rgb
{
	const GLfloat red[4]		= { 1.0f, 0.0f, 0.0f, 1.0f };
	const GLfloat green[4]		= { 0.0f, 1.0f, 0.0f, 1.0f };
	const GLfloat blue[4]		= { 0.0f, 0.0f, 1.0f, 1.0f };
	const GLfloat grey[4]		= { 0.215f, 0.215f, 0.215f, 1.0f };
	const GLfloat ligthGrey[4]	= { 0.75f, 0.75f, 0.75f, 1.0f };
	const GLfloat black[4]		= { 0.0f, 0.0f, 0.0f, 1.0f };
	const GLfloat white[4]		= { 1.0f, 1.0f, 1.0f, 1.0f };
}

namespace GL
{
	void setupOrtho();
	void restoreGL();

	void drawFilledRect(float x, float y, float width, float height, const GLfloat color[4]);
	void drawOutlineRect(float x, float y, float width, float height, float lineWidth, const GLfloat color[4]);
	void drawOutlineCircle(float x, float y, float radius, int numSegments, float lineWidth, const GLfloat color[4]);
}
