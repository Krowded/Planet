#ifndef UPDATE_AND_DISPLAY_H
#define UPDATE_AND_DISPLAY_H

#include "Init.h"

void display(void);

//Update position and rotation of camera
void CamUpdate(GLint mouseX, GLint mouseY);
void UpdatePosition(GLint t);

void timer(GLint integer);

#endif