#ifndef UPDATE_AND_DISPLAY_H
#define UPDATE_AND_DISPLAY_H

#include "VectorUtils3.h"

#define LOCAL static

void display(void);

//Update position and rotation of camera
void MouseUpdate(GLint mouseX, GLint mouseY);
LOCAL void Update(GLint t);

void timer(GLint integer);

#endif