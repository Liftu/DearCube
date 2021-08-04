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

void GL::drawFilledRect(float xLeft, float yUp, float xRight, float yLow, const GLfloat color[4])
{
	glColor4f(color[0], color[1], color[2], color[3]);
	glBegin(GL_QUADS);
	// Starts top left and go clock wise
	glVertex2f(xLeft, yUp);		// Top left
	glVertex2f(xRight, yUp);	// Top right
	glVertex2f(xRight, yLow);	// Bottom right
	glVertex2f(xLeft, yLow);	// Bottom left
	glEnd();
}

void GL::drawOutlineRect(float xLeft, float yUp, float xRight, float yLow, float lineWidth, const GLfloat color[4])
{
	glLineWidth(lineWidth);
	glColor4f(color[0], color[1], color[2], color[3]);
	glBegin(GL_LINE_STRIP);
	// Starts top left and go clock wise
	glVertex2f(xLeft - 0.5f, yUp - 0.5f);	// Top left
	glVertex2f(xRight + 0.5f, yUp - 0.5f);	// Top right
	glVertex2f(xRight + 0.5f, yLow + 0.5f);	// Bottom right
	glVertex2f(xLeft - 0.5f, yLow + 0.5f);	// Bottom left
	glVertex2f(xLeft - 0.5f, yUp - 0.5f);	// Top left again
	glEnd();
}

void GL::drawOutlineCircle(float x, float y, float radius, int numSegments, float lineWidth, const GLfloat color[4])
{
	glLineWidth(lineWidth);
	glColor4f(color[0], color[1], color[2], color[3]);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < numSegments; i++) {
		float theta = 2.0f * PI * float(i) / float(numSegments);	//get the current angle 
		float segmentX = radius * cosf(theta);	//calculate the x component 
		float segmentY = radius * sinf(theta);	//calculate the y component 
		glVertex2f(segmentX + x, segmentY + y);	//output vertex 
	}
	glEnd();
}