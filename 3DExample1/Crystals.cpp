#include "Crystals.h"

Crystals::Crystals(GLint newxPos, GLint newyPos, GLint newzPos)
{
	xPos = newxPos;
	yPos = newyPos;
	zPos = newzPos;
}

void Crystals::drawCrystal(GLUquadricObj* quadricCrystal, GLuint texture, GLfloat rotation, std::vector<Crystals>& crystalsVec, GLfloat lightPos[])
{
	quadricCrystal;
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glPushMatrix();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glFrontFace(GL_CCW);
	glTranslatef(xPos, yPos, zPos);
	glRotatef(6 * rotation, 0.0f, 1.0f, 0.0f);
	glTranslatef(-xPos, -yPos, -zPos);
	glTranslatef(xPos, yPos, zPos);
	glEnable(GL_TEXTURE_2D);
	quadricCrystal = gluNewQuadric();
	glColor4f(r, g, b, 0.7);
	glutSolidIcosahedron();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glPopMatrix();
}

void Crystals::collisionCrystal(std::vector<Crystals>& crystalsVec, GLfloat& score, GLfloat& camX, GLfloat& camY, GLfloat& camZ)
{
	for (int i = 0; i < crystalsVec.size(); i++)
	{
		if (sqrt(pow(crystalsVec[i].xPos - camX, 2) + pow(crystalsVec[i].yPos - camY, 2) + pow(crystalsVec[i].zPos - camZ, 2)) < 4) // Checks distance between camera and crystal
		{
			crystalsVec.erase(crystalsVec.begin() + i);
			score += 1.0; 
		}
	}
}

Crystals::~Crystals(){}