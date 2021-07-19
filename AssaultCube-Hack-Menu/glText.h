#pragma once
#pragma comment(lib, "Opengl32.lib")
#include "pch.h"
#include <stdio.h>
#include <gl/GL.h>

struct Vec3 { float x, y, z; };

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
		void print(float x, float y, const unsigned char color[3], const char* format, ...);

		Vec3 centerText(float x, float y, float width, float height, float textWidth, float textHeight);
		float centerText(float x, float width, float textWidth);
	};
}