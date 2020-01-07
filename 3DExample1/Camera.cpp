#include "Camera.h"

void Camera::calculateVectors()
{
		verV[X] = dir[X] - pos[X];
		verV[Z] = dir[Z] - pos[Z];
		verV[Y] = dir[Y] - pos[Y];
		GLfloat mag = sqrt(verV[X] * verV[X] + verV[Z] * verV[Z] + verV[Y] * verV[Y]);
		verV[X] /= mag;
		verV[Z] /= mag;
		verV[Y] /= mag;
		horV[X] = verV[Z];
		horV[Z] = verV[X];
		horV[Y] = verV[Y];
}