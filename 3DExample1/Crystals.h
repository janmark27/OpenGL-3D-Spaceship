#pragma once
#include <math.h>						
#include <gl/freeglut.h>
#include <vector>

class Crystals
{
public:
	GLfloat xPos,yPos,zPos, r = 0.0f, g = 0.0f, b = 0.0f;
	Crystals(GLint newxPos, GLint newyPos, GLint newzPos);
	~Crystals();
	void drawCrystal(GLUquadricObj* quadricCrystal, GLuint texture, GLfloat rotation, std::vector<Crystals>& crystalsVec, GLfloat lightPos[]);
	void collisionCrystal(std::vector<Crystals>& crystalsVec, GLfloat& score, GLfloat& camX, GLfloat& camY, GLfloat& camZ);
};