#pragma once
#pragma comment(lib, "Opengl32.lib")
#include "pch.h"
#include <stdio.h>
#include <gl/GL.h>

namespace rgb
{
	const GLubyte red[3]		= { 255, 0, 0 };
	const GLubyte green[3]		= { 0, 255, 0 };
	const GLubyte blue[3]		= { 0, 0, 255 };
	const GLubyte grey[3]		= { 55, 55, 55 };
	const GLubyte ligthGrey[3]	= { 192, 192, 192 };
	const GLubyte black[3]		= { 0, 0, 0 };
}

namespace GL
{
	void setupOrtho();
	void restoreGL();

	void drawFilledRect(float x, float y, float width, float height, const GLubyte color[3]);
	void drawOutline(float x, float y, float width, float height, float lineWidth, const GLubyte color[3]);
}
