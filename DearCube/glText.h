#pragma once
#pragma comment(lib, "Opengl32.lib")

#include <Windows.h>
#include <stdio.h>
#include <gl/GL.h>

#include "geom.h"

using Geom::Vector2;

namespace GL
{
	class Font
	{
	public:
		bool bBuilt = false;
		unsigned int base;
		HDC hDc = nullptr;
		int height;
		int width;

		void build(int height);
		void print(float x, float y, const GLfloat color[4], const char* format, ...);

		Vector2 centerText(float x, float y, float width, float height, float textWidth, float textHeight);
		float centerText(float x, float width, float textWidth);
	};
}