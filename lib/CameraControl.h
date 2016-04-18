#ifndef CAMERA_CONTROL_H
#define CAMERA_CONTROL_H

#include "MicroGlut.h"
#include "VectorUtils3.h"
#include "Physics.h"
#include <stdlib.h>
#include "Init.h"


void CameraControlsInit(GLfloat standarSpeed, GLfloat runSpeed, GLint windowWidth, GLint windowHeight);
vec3 GetCurrentCameraPosition(mat4 camBaseMatrix);
vec3 GetBackDirectionVec(mat4 camMatrix);
vec3 GetRightDirectionVec(mat4 camMatrix);
vec3 GetUpDirectionVec(mat4 camMatrix);
vec3 GetNewUpDirectionVec(mat4 camMatrix);
mat4 SetUpDirectionVec(mat4 camMatrix, vec3 newUpDirection);

mat4 CameraControl(GLint t, mat4 camMatrix, mat4 camBaseMatrix);
mat4 CameraMouseUpdate(GLint x, GLint y, mat4 camMatrix, mat4 camBaseMatrix);
void CamUpdate(GLint mouseX, GLint mouseY);
void UpdatePosition(GLint t);

#endif