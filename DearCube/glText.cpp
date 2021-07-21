#include "glText.h"

void GL::Font::build(int height)
{
	hDc = wglGetCurrentDC();
	base = glGenLists(96);
	HFONT hFont = CreateFontA(-height, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, ANSI_CHARSET, 
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_DONTCARE | DEFAULT_PITCH, "Consolas");
	HFONT hOldFont = (HFONT)SelectObject(hDc, hFont);
	wglUseFontBitmaps(hDc, 32, 96, base);
	SelectObject(hDc, hOldFont);
	DeleteObject(hFont);

	bBuilt = true;
}

void GL::Font::print(float x, float y, const unsigned char color[3], const char* format, ...)
{
	glColor3ub(color[0], color[1], color[2]);
	glRasterPos2f(x, y);

	char text[100];
	// Variable arguments
	va_list args;
	va_start(args, format);
	vsprintf_s(text, 100, format, args);
	va_end(args);

	glPushAttrib(GL_LIST_BIT);
	glListBase(base - 32);
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();
}

Vec3 GL::Font::centerText(float x, float y, float width, float height, float textWidth, float textHeight)
{
	Vec3 text;
	text.x = x + (width - textWidth) / 2;
	text.y = y + textHeight;
	return text;
}

float GL::Font::centerText(float x, float width, float textWidth)
{
	if (width > textWidth)
		return (x + ((width - textWidth) / 2));
	else
		return (x - ((textWidth - width) / 2));
}