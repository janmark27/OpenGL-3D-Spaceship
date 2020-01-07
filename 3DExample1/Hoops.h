#pragma once
#include <math.h>						
#include <gl/freeglut.h>
#include <vector>

#define PI 3.1415926535

class Hoops
{
public:
	std::vector<GLfloat> colliderHoopX, colliderHoopY, colliderHoopZ;
	GLfloat hoopXPos, hoopYPos, hoopZPos;
	Hoops(GLint newxPos, GLint newyPos, GLint newzPos);
	~Hoops();
	void drawHoop(GLUquadricObj* quadricCrystal, GLuint texture, GLfloat rotation);
	void drawHoopTexture(GLfloat rotation, GLfloat iRad, GLfloat oRad, GLint sides, GLint rings);
	void collisionHoop(std::vector<Hoops>& hoopsVec, GLfloat& camX, GLfloat& camY, GLfloat& camZ, GLfloat radius, bool gameOn);
};