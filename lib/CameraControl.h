#ifndef CAMERA_CONTROL_H
#define CAMERA_CONTROL_H

#include "VectorUtils3.h"
#include "Physics.h"
#include "ControlGlobals.h"
#include "Init.h"

vec3 GetCurrentCameraPosition(mat4 camPositionMatrix);
vec3 GetBackDirectionVec(mat4 camRotatedMatrix);
vec3 GetRightDirectionVec(mat4 camRotatedMatrix);
vec3 GetUpDirectionVec(mat4 camRotatedMatrix);
vec3 GetOldUpDirectionVec(mat4 camRotatedMatrix);
vec3 GetNewUpDirectionVec(mat4 camRotatedMatrix);
mat4 ChangeUpDirection(mat4 camRotatedMatrix, vec3 newUpDirection);

mat4 CameraControl(GLint t, mat4 camRotatedMatrix, mat4 camPositionMatrix);
void CameraMouseUpdate(GLint x, GLint y);
mat4 UpdateCamera(mat4 camPositionMatrix);
void UpdatePosition(GLint t);
mat4 AdjustCameraToHeightMap(mat4 camPositionMatrix, GLfloat height);

#endif