#pragma once
#include <gl/freeglut.h>
#include <math.h>						

enum Axis {
	X, Y, Z    
};

class Camera {
public:
	GLfloat pos[3];     // Ship position
	GLfloat dir[3];     // Looking direction
	GLfloat verV[3];	// vertical vector
	GLfloat horV[3];	// horizontal vector
	GLfloat upV[3];     // up vector

	Camera() {
		pos[X] = 0; pos[Y] = 0; pos[Z] = 0;
		dir[X] = 0; dir[Y] = 0; dir[Z] = 100; 
		upV[X] = 0; upV[Y] = 1; upV[Z] = 0;
		calculateVectors();
	}
	void calculateVectors();
};