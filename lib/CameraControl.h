#ifndef CAMERA_CONTROL_H
#define CAMERA_CONTROL_H

#include "MicroGlut.h"
#include "VectorUtils3.h"
#include "Physics.h"
#include <stdlib.h>
#include "ControlGlobals.h"
#include "Init.h"

vec3 GetCurrentCameraPosition(mat4 camPositionMatrix);
vec3 GetBackDirectionVec(mat4 camRotatedMatrix);
vec3 GetRightDirectionVec(mat4 camRotatedMatrix);
vec3 GetUpDirectionVec(mat4 camRotatedMatrix);
vec3 GetNewUpDirectionVec(mat4 camRotatedMatrix);
mat4 ChangeUpDirection(mat4 camRotatedMatrix, vec3 newUpDirection);

mat4 CameraControl(GLint t, mat4 camRotatedMatrix, mat4 camPositionMatrix);
mat4 CameraMouseUpdate(GLint x, GLint y, mat4 camRotatedMatrix, mat4 camPositionMatrix);
void CamUpdate(GLint mouseX, GLint mouseY);
void UpdatePosition(GLint t);
mat4 AdjustCameraToHeightMap(mat4 camPositionMatrix, GLfloat height);

#endif