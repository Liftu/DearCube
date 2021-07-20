#include "glDraw.h"

void GL::setupOrtho()
{
	// I didn't write this, I don't know how it work.
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	GLint viewport[4];	// window : 0 = x, 1 = y, 2 = width, 3 = heigth
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, viewport[2], viewport[3]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, viewport[2], viewport[3], 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
}

void GL::restoreGL()
{
	glPopMatrix();
	glPopAttrib();
}

void GL::drawFilledRect(float x, float y, float width, float height, const GLubyte color[3])
{
	glColor3ub(color[0], color[1], color[2]);
	glBegin(GL_QUADS);
	// Starts top left and go clock wise
	glVertex2f(x, y);					// Top left
	glVertex2f(x + width, y);			// Top right
	glVertex2f(x + width, y + height);	// Bottom right
	glVertex2f(x, y + height);			// Bottom left
	glEnd();
}

void GL::drawOutline(float x, float y, float width, float height, float lineWidth, const GLubyte color[3])
{
	glLineWidth(lineWidth);
	glColor3ub(color[0], color[1], color[2]);
	glBegin(GL_LINE_STRIP);
	// Starts top left and go clock wise
	glVertex2f(x - 0.5f, y - 0.5f);						// Top left
	glVertex2f(x + width + 0.5f, y - 0.5f);				// Top right
	glVertex2f(x + width + 0.5f, y + height + 0.5f);	// Bottom right
	glVertex2f(x - 0.5f, y + height + 0.5f);			// Bottom left
	glVertex2f(x - 0.5f, y - 0.5f);						// Top left again
	glEnd();
}
