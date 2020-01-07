#include <math.h>						
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>	//needed for console output (debugging)
#include <gl/freeglut.h>
#include <iostream>
#include <vector>
#include "3DExample1/Camera.h"
#include "3DExample1/Crystals.h"
#include "3DExample1/Hoops.h"

#ifdef WIN32
#include "gltools.h"
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "glm.h"
#endif

using namespace std;

#define PI 3.1415926535
#define IMAGE1      0
#define IMAGE2      1
#define IMAGE3      2
#define IMAGE4      3
#define TEXTURE_BALL 4 
#define TEXTURE_COUNT 5	

GLuint  textures[TEXTURE_COUNT];

const char* textureFiles[TEXTURE_COUNT] = { "hud.tga", "horse.tga","tycho_cyl_glow.tga","../3DExample1/starField.tga","jupiterC.tga" };

GLfloat Wwidth, Wheight,jupiterOrbit = 0, rotation = 0, rollAngle = 0, pitchAngle = 0, yawAngle = 0, score = 0.0, hudR = 0.7f, hudG = 0.2f, hudB = 0.8f;

GLfloat mKa[4] = { 0.11f,0.06f,0.11f,1.0f }; //ambient
GLfloat mKd[4] = { 0.43f,0.47f,0.54f,1.0f }; //diffuse
GLfloat mKs[4] = { 1.0f,1.0f,1.0f,1.0f }; //specular
GLfloat mKe[4] = { 0.5f,0.5f,0.0f,1.0f }; //emission

GLfloat	 lightPos[] = { 20.0, 20.0, 0.0, 1.0f };
GLfloat  spotDir[] = { 50.0, 25.0, 0.0 };
GLfloat	 lightPos2[] = { 50.0, 100.0, 300.0, 0.0f };
GLfloat  spotDir2[] = { 50.0, 15.0, 0.0 };
GLfloat	 lightPos3[] = { -50.0, 100.0, 300.0, 0.0f };
GLfloat  spotDir3[] = { 50.0, 15.0, 0.0 };

GLfloat  whiteLightBright[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat  redLight[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat  greenLight[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat  blueLight[] = { 0.0f, 1.0f, 1.0f, 1.0f };
GLfloat  whiteLightLessBright[] = { 0.8f, 0.8f, 0.8f, 1.0f };

GLint iWidth, iHeight, iComponents;
GLenum eFormat;
GLbyte* pBytes0;

std::vector<GLfloat> obstaclexPos = {-5, 0, 5, 10, 5, 0},		// Coordinates used for the Hoops and Crystals
					 obstacleyPos = {0 , 7 , 14, 21, 14, 7},
					 obstaclezPos = {15, 35, 55, 75, 95, 115};

Camera cam;
std::vector<Crystals> crystalsVec;
std::vector<Hoops> hoopsVec;
GLUquadricObj* quadricSpace;
GLUquadricObj* quadricPlanet;
GLUquadricObj* quadricCrystal;
GLUquadricObj* quadricHoop;

bool gameOn = true;
bool switchHUD = false;

void setOrthographicProjection() {			// Change Projection to  2D
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, Wwidth,0, Wheight);
	glScalef(1, -1, 1);
	glTranslatef(0, -Wheight, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
} 

void resetPerspectiveProjection()			// Reset Projection back to 3D
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
} 

void drawHUD(int image)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Makes HUD transparent 
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[image]); // Adds HUD texture
	glColor4f(1.0f, 1.0f, 1.0f, 0.20f);
	glBegin(GL_QUADS); // Draws HUD
	glTexCoord2f(0.0, 1.0); glVertex2f(0.0, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex2f(Wwidth, 0.0);// Makes sure the HUD is always the size of the window
	glTexCoord2f(1.0, 0.0); glVertex2f(Wwidth, Wheight);
	glTexCoord2f(0.0, 0.0); glVertex2f(0.0, Wheight);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND); 
}

void lettersHUD(void* font, char* string, GLfloat x, GLfloat y, GLfloat r, GLfloat g, GLfloat b) // Write a bitmap character string 
{
	char* c;
	glColor3f(r, g, b);
	glRasterPos2i(x, y);
	for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

void numbersHUD(void* font, GLfloat& number, GLfloat x, GLfloat y, GLfloat r, GLfloat g, GLfloat b) // Transform the Score variable to a string
{
	char buffer[4];
	GLint ret = snprintf(buffer, sizeof buffer, "%f", number);
	lettersHUD(font, buffer,x , y, r, g, b); // Writes the current score
}

void drawPlanet(GLfloat x, GLfloat y, GLfloat z, GLfloat r)
{
	glPushMatrix();
	glFrontFace(GL_CCW);
	glRotatef(jupiterOrbit, 0.0, 1.0, 0.0); //Jupiter Orbit
	glTranslatef(0.0, 30.0, 200.0);
	glRotatef(90.0f, -1.0, 0.0, 0.0); // Repositions Jupiter straight
	glRotatef(-2 * rotation, 0.0, 0.0, 1.0); //Jupiter Rotation
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_BALL]); // Adds jupiter texture
	glColor3f(0.5, 0.5, 0.5);
	quadricPlanet = gluNewQuadric();
	gluQuadricDrawStyle(quadricPlanet, GLU_FILL);
	gluQuadricNormals(quadricPlanet, GLU_SMOOTH);
	gluQuadricOrientation(quadricPlanet, GLU_OUTSIDE);
	gluQuadricTexture(quadricPlanet, GL_TRUE);
	gluSphere(quadricPlanet, r, 50, 35);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void drawSpace(GLfloat x, GLfloat y, GLfloat z, GLfloat r)
{
	glPushMatrix();
	glFrontFace(GL_CCW);
	glScalef(-1.0f, -1.0f, -1.0f); // Inverses sphere to texture inside
	glRotatef(120.0f, 1.0f, -0.2f, 0.7f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[IMAGE3]); // textures sphere
	glColor3f(0.5, 0.5, 0.5);
	quadricSpace = gluNewQuadric();
	gluQuadricDrawStyle(quadricSpace, GLU_FILL);
	gluQuadricNormals(quadricSpace, GLU_SMOOTH);
	gluQuadricOrientation(quadricSpace, GLU_OUTSIDE);
	gluQuadricTexture(quadricSpace, GL_TRUE);
	gluSphere(quadricSpace, r, 50, 35);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void spawnCrystals()
{
	for (size_t i = 0; i < obstaclexPos.size(); i++)
	{
		crystalsVec.push_back(Crystals(obstaclexPos[i], obstacleyPos[i], obstaclezPos[i]));
	}
}

void spawnHoops()
{
	for (size_t i = 0; i < obstaclexPos.size(); i++)
	{
		hoopsVec.push_back(Hoops(obstaclexPos[i], obstacleyPos[i], obstaclezPos[i]));
	}
}

void TimerFunc(int value)
{
	jupiterOrbit += 0.0007;
	if (jupiterOrbit > 360.0f)
	{
		jupiterOrbit = 0;
	}
	rotation += 0.003;
	glutPostRedisplay();
}

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	
	gluLookAt(cam.pos[X], cam.pos[Y], cam.pos[Z], cam.dir[X], cam.dir[Y], // Camera Movement
		cam.dir[Z], cam.upV[X], cam.upV[Y], cam.upV[Z]);

	glPushMatrix();
	drawSpace(0.0, 0.0, 0.0, 2000.0);
	drawPlanet(0.0, 0.0, 10.0, 10.0);
	
	for (Crystals& a : crystalsVec) // draws and checks collision for every crystal saved in the class vector
	{
		for (int i = 0; i < crystalsVec.size(); i++)
		{
			if (i == 0){crystalsVec[i].r = 0.0; crystalsVec[i].g = 1.0; crystalsVec[i].b = 0.0;}
			else {crystalsVec[i].r = 1.0; crystalsVec[i].g = 0.7; crystalsVec[i].b = 0.0;}
		}
		a.collisionCrystal(crystalsVec, score, cam.pos[X], cam.pos[Y], cam.pos[Z]);
		TimerFunc(rotation);
		a.drawCrystal(quadricCrystal,textures[IMAGE4],rotation,crystalsVec, lightPos);
	}
	for (Hoops& a : hoopsVec) // draws for every hoop saved in the class vector
	{
		//a.collisionHoop(hoopsVec, score, cam.pos[X], cam.pos[Y], cam.pos[Z], 4, gameOn);
		TimerFunc(rotation);
		a.drawHoop(quadricHoop, textures[IMAGE4], rotation);
	}
	glPopMatrix();
	
	setOrthographicProjection(); // CHanges to 2D

	if (switchHUD)		// Writes coordinates and HUD if toggle is active
	{
		glColor3f(hudR, hudG, hudB);
		lettersHUD(GLUT_BITMAP_HELVETICA_18, "CRYSTALS COLLECTED",       Wwidth / 2 - 98, 20, hudR, hudG, hudB);
		numbersHUD(GLUT_BITMAP_HELVETICA_18, score,                       Wwidth / 2 - 8, 50, hudR, hudG, hudB);
		lettersHUD(GLUT_BITMAP_HELVETICA_18, "X POSITION",              10, Wheight / 2 - 60, hudR, hudG, hudB);
		numbersHUD(GLUT_BITMAP_HELVETICA_18, cam.pos[X],               150, Wheight / 2 - 60, hudR, hudG, hudB);
		lettersHUD(GLUT_BITMAP_HELVETICA_18, "Y POSITION",              10, Wheight / 2 - 20, hudR, hudG, hudB);
		numbersHUD(GLUT_BITMAP_HELVETICA_18, cam.pos[Y],               150, Wheight / 2 - 20, hudR, hudG, hudB);
		lettersHUD(GLUT_BITMAP_HELVETICA_18, "Z POSITION",              10, Wheight / 2 + 20, hudR, hudG, hudB);
		numbersHUD(GLUT_BITMAP_HELVETICA_18, cam.pos[Z],               150, Wheight / 2 + 20, hudR, hudG, hudB);
		lettersHUD(GLUT_BITMAP_HELVETICA_18, "X ROTATION",    Wwidth - 125, Wheight / 2 - 60, hudR, hudG, hudB);
		numbersHUD(GLUT_BITMAP_HELVETICA_18, pitchAngle,      Wwidth - 175, Wheight / 2 - 60, hudR, hudG, hudB);													           	
		lettersHUD(GLUT_BITMAP_HELVETICA_18, "Y ROTATION",    Wwidth - 125, Wheight / 2 - 20, hudR, hudG, hudB);
		numbersHUD(GLUT_BITMAP_HELVETICA_18, yawAngle,        Wwidth - 175, Wheight / 2 - 20, hudR, hudG, hudB);									           	
		lettersHUD(GLUT_BITMAP_HELVETICA_18, "Z ROTATION",    Wwidth - 125, Wheight / 2 + 20, hudR, hudG, hudB);
		numbersHUD(GLUT_BITMAP_HELVETICA_18, rollAngle,       Wwidth - 175, Wheight / 2 + 20, hudR, hudG, hudB);
		drawHUD(IMAGE1);
	}

	resetPerspectiveProjection(); // Resets back to 3D

	glutSwapBuffers();
}

void SetupRC()
{
	GLuint texture;
	glGenTextures(1, &texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(TEXTURE_COUNT, textures);
	for (int iLoop = 0; iLoop < TEXTURE_COUNT; iLoop++)
	{
		glBindTexture(GL_TEXTURE_2D, textures[iLoop]);

		pBytes0 = gltLoadTGA(textureFiles[iLoop], &iWidth, &iHeight,
			&iComponents, &eFormat);

		glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		free(pBytes0);
	}
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);	// Hidden surface removal    
	glFrontFace(GL_CCW);// Counter clock-wise polygons face out

	glEnable(GL_LIGHTING);
	glEnable(GL_POINT_SMOOTH);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteLightBright);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLightBright);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glLightfv(GL_LIGHT0, GL_SPECULAR, mKs);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 60.0f);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_DIFFUSE, whiteLightBright);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);
	glLightfv(GL_LIGHT1, GL_SPECULAR, mKs);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 60.0f);
	//glEnable(GL_LIGHT1);

	glLightfv(GL_LIGHT2, GL_DIFFUSE, whiteLightBright);
	glLightfv(GL_LIGHT2, GL_POSITION, lightPos3);
	glLightfv(GL_LIGHT2, GL_SPECULAR, mKs);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 60.0f);
	//glEnable(GL_LIGHT2);

	// Enable colour tracking
	glEnable(GL_COLOR_MATERIAL);

	// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// All materials hereafter have full specular reflectivity
	// with a high shine
	glMaterialfv(GL_FRONT, GL_SPECULAR, mKs);
	glMateriali(GL_FRONT, GL_SHININESS, 128);

	// Black blue background clear colour
	glClearColor(0.0f, 0.0f, 0.03f, 1.0f);

	glMatrixMode(GL_PROJECTION);       // Switch  to projection matrix
	glLoadIdentity();                  // Clear current matrix to identity matrix
	gluPerspective(80, 1, 1, 1000000); // Set projection of camera

	glMatrixMode(GL_MODELVIEW);        // Switch  to modelview matrix
	glLoadIdentity();                  // Clear current matrix to identity matrix

	gluLookAt(cam.pos[X], cam.pos[Y], cam.pos[Z], cam.dir[X], cam.dir[Y],
		cam.dir[Z], 0, 1, 0);     // Set camera position and direction
}

void popUpMenuActions(int option) {

	switch (option) {
	case 1:
		switchHUD = !switchHUD; // Turn HUD ON/OFF
		break;
	}
}

void PopUpMenu() {

	int popUpMenu;

	popUpMenu = glutCreateMenu(popUpMenuActions); //Event handling

	glutAddMenuEntry("HUD ON/OFF", 1); //Entries
	glutRemoveMenuItem(2);

	glutAttachMenu(GLUT_RIGHT_BUTTON); // Trigger with right click
}

void keyInput(unsigned char key, int x, int y){
	switch (key)
	{
	case 'w':						//Move Forwards
		cam.pos[X] += cam.verV[X];
		cam.pos[Z] += cam.verV[Z];
		cam.dir[X] += cam.verV[X];
		cam.dir[Z] += cam.verV[Z];
		break;

	case 'a':						//Move Right
		cam.pos[X] += cam.horV[X];
		cam.pos[Z] += cam.horV[Z];
		cam.dir[X] += cam.horV[X];
		cam.dir[Z] += cam.horV[Z];
		break;

	case 's':						//Move Backwards
		cam.pos[X] -= cam.verV[X];
		cam.pos[Z] -= cam.verV[Z];
		cam.dir[X] -= cam.verV[X];
		cam.dir[Z] -= cam.verV[Z];
		break;

	case 'd':						//Move Left
		cam.pos[X] -= cam.horV[X];
		cam.pos[Z] -= cam.horV[Z];
		cam.dir[X] -= cam.horV[X];
		cam.dir[Z] -= cam.horV[Z];
		break;

	case 'c':				// Move Up
		cam.pos[Y] += 1.0f;
		cam.dir[Y] += 1.0f;
		break;

	case 'z':				//Move Down
		cam.pos[Y] -= 1.0f;
		cam.dir[Y] -= 1.0f;
		break;

	case 'e':				//Roll Right
		rollAngle += 3.0f;
		if (rollAngle > 360)
		{
			rollAngle = 0;
		}
		cam.upV[X] = (cosf(rollAngle * PI / 180.0));
		cam.upV[Y] = (sinf(rollAngle * PI / 180.0));
		break;

	case 'q':				//Roll Left
		rollAngle -= 3.0f;

		if (rollAngle < -360)
		{
			rollAngle = 0;
		}
		cam.upV[X] = (cosf(rollAngle * PI / 180.0));
		cam.upV[Y] = (sinf(rollAngle * PI / 180.0));
		break;

	case 'i':				//Pitch Forwards
		pitchAngle -= 3.0f;
		if (pitchAngle < -360)
		{
			pitchAngle = 0;
		}
		cam.dir[Y] = sinf(pitchAngle * PI / 180) + cam.pos[Y];
		cam.dir[Z] = cosf(pitchAngle * PI / 180) + cam.pos[Z];
		break;

	case 'j':				//Yaw Left
		yawAngle += 3.0f;
		if (yawAngle < -360)
		{
			yawAngle = 0;
		}
		cam.dir[X] = sinf(yawAngle * PI / 180) + cam.pos[X];
		cam.dir[Z] = cosf(yawAngle * PI / 180) + cam.pos[Z];
		break;

	case 'k':				//Pitch Backwards
		pitchAngle += 3.0f;
		if (pitchAngle > 360)
		{
			pitchAngle = 0;
		}
		cam.dir[Y] = sinf(pitchAngle * PI / 180) + cam.pos[Y];
		cam.dir[Z] = cosf(pitchAngle * PI / 180) + cam.pos[Z];
		break;

	case 'l':				//Yaw Right
		yawAngle -= 3.0f;
		if (yawAngle < -360)
		{
			yawAngle = 0;
		}
		cam.dir[X] = sinf(yawAngle * PI / 180) + cam.pos[X];
		cam.dir[Z] = cosf(yawAngle * PI / 180) + cam.pos[Z];
		break;
	
	case 'h':				// HUD ON/OFF
		switchHUD = !switchHUD;
	}
	glutPostRedisplay();
}

void ChangeSize(int w, int h)
{
	GLfloat fAspect;

	// Prevent a divide by zero
	if (h == 0)
		h = 1;
	Wwidth = w;
	Wheight = h;
	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);

	// Calculate aspect ratio of the window
	fAspect = (GLfloat)w / (GLfloat)h;

	// Set the perspective coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// field of view of 45 degrees, near and far planes 1.0 and 1000
	//that znear and zfar should typically have a ratio of 1000:1 to make sorting out z depth easier for the GPU
	gluPerspective(55.0f, fAspect, 1.0, 3000.0);

	// Modelview matrix reset
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char* argv[])
{
	cout << "                     OpenGL 3D Space Game " << endl;
	cout << "     Your objective is to move your first person spaceship" << endl;
	cout << "through space to collect the crystals through the spinning hoops" << endl << endl;
	cout << "Controls" << endl << endl;
	cout << "W:           Move Forwards" << endl;
	cout << "A:           Move Left " << endl;
	cout << "S:           Move Right" << endl;
	cout << "D:           Move Backwards" << endl;
	cout << "C:           Move Up" << endl;
	cout << "Z:           Move Down" << endl;
	cout << "Q:           Left Roll" << endl;
	cout << "E:           Right Roll" << endl;
	cout << "I:           Rotate Forwards" << endl;
	cout << "J:           Rotate Left" << endl;
	cout << "Q:           Rotate Backwards" << endl;
	cout << "L:           Rotate Right" << endl;
	cout << "H:           Turn the HUD ON/OFF" << endl;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1400, 900);
	glutCreateWindow("Textures Tutorial");
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
	glutKeyboardFunc(keyInput);
	glutTimerFunc(25, TimerFunc, 1);
	PopUpMenu();
	SetupRC();
	spawnCrystals();
	spawnHoops();
	glutMainLoop();
	return 0;
}