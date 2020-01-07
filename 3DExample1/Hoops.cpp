#include "Hoops.h"

Hoops::Hoops(GLint newxPos, GLint newyPos, GLint newzPos)
{
	hoopXPos = newxPos;
	hoopYPos = newyPos;
	hoopZPos = newzPos;
}

void Hoops::drawHoop(GLUquadricObj* quadricHoops, GLuint texture, GLfloat rotation)
{
	quadricHoops;
	glPushMatrix();
	glFrontFace(GL_CCW);
	glTranslatef(hoopXPos, hoopYPos, hoopZPos);
	glRotatef(-3 * rotation, 0.0f, 1.0f, 0.0f);
	glRotatef(3 * rotation, 0.0f, 0.0f, 1.0f);
	glTranslatef(-hoopXPos, -hoopYPos, -hoopZPos);
	glTranslatef(hoopXPos, hoopYPos, hoopZPos);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glColor3f(0.5, 0.5, 0.5);
	quadricHoops = gluNewQuadric();
	gluQuadricDrawStyle(quadricHoops, GLU_FILL);
	gluQuadricNormals(quadricHoops, GLU_SMOOTH);
	gluQuadricOrientation(quadricHoops, GLU_OUTSIDE);
	gluQuadricTexture(quadricHoops, GL_TRUE);
	glutSolidTorus(0.37f, 4.0f, 30.0f, 30.0f);
	drawHoopTexture(rotation, 0.2f, 2.0f, 30.0f, 30.0f);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void Hoops::drawHoopTexture(GLfloat rotation, GLfloat iRad, GLfloat oRad, GLint sides, GLint rings)
{
	glFrontFace(GL_CW);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	const GLfloat CIRCLE = 2 * PI;

	for (int i = 0; i < sides; i++) { // Draws circles through a bigger circumference to make a ring
		GLfloat x, y, z;
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= rings; j++) {
			for (int k = 0; k <= 1; k++) {
				GLfloat s = (i + k) % sides + 0.5;
				GLfloat t = j % (rings + 1);

				x = (oRad + iRad * cos(s * CIRCLE / sides)) * cos(t * CIRCLE / rings);
				y = (oRad + iRad * cos(s * CIRCLE / sides)) * sin(t * CIRCLE / rings);
				z = iRad * sin(s * CIRCLE / sides);

				GLfloat u = (i + k) / (float)sides;
				GLfloat v = t / (float)rings;

				glTexCoord2d(u, v);
				glNormal3f(2 * x, 2 * y, 2 * z);
				glVertex3f(2 * x, 2 * y, 2 * z);
			}
		}
		glEnd();
	}
	glFrontFace(GL_CCW);
	glDisable(GL_TEXTURE_2D);;
}

void Hoops::collisionHoop(std::vector<Hoops>& hoopsVec, GLfloat& camX, GLfloat& camY, GLfloat& camZ, GLfloat radius,  bool gameOn)
{
	for (int i = 0; i < hoopsVec.size(); i++)
	{
		for (int angle = 0; angle <= 360; angle + 60)
		{
			colliderHoopX.push_back(hoopXPos + radius * cosf(angle * PI / 180));
			colliderHoopY.push_back(hoopYPos + radius * sinf(angle * PI / 180));
			colliderHoopZ.push_back(hoopZPos + radius * cosf(angle * PI / 180));
		}
		for(int j = 0; j < colliderHoopX.size(); j++)
		{
			if (sqrt(pow(hoopsVec[i].colliderHoopX[j] - camX, 2) + pow(hoopsVec[i].colliderHoopY[j] - camY, 2) + pow(hoopsVec[i].colliderHoopZ[j] - camZ, 2)) < 7) // Calculating if the distance between the bullet and asteroid is less than the sum of their radiuses
			{
				gameOn = !gameOn;
			}
		}
	}
	
}
Hoops::~Hoops(){}