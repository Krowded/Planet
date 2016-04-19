#ifndef UPDATE_AND_DISPLAY_H
#define UPDATE_AND_DISPLAY_H

#include "MicroGlut.h"
#include "DisplayGlobals.h"
#include "Init.h"
#include "VectorUtils3.h"
#include "CameraControl.h"

void display(void);

//Update position and rotation of camera
void CamUpdate(GLint mouseX, GLint mouseY);
void UpdatePosition(GLint t);

void timer(GLint integer);

#endif