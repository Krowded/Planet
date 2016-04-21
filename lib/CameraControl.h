#ifndef CAMERA_CONTROL_H
#define CAMERA_CONTROL_H

#include "VectorUtils3.h"
#include "Physics.h"
#include "ControlGlobals.h"
#include "DisplayGlobals.h"
#include "Init.h"

#define LOCAL static

vec3 GetCurrentCameraPosition(mat4 camPositionMatrix);
LOCAL vec3 GetBackDirectionVec(mat4 camRotatedMatrix);
LOCAL vec3 GetRightDirectionVec(mat4 camRotatedMatrix);
LOCAL vec3 GetUpDirectionVec(mat4 camRotatedMatrix);
LOCAL vec3 GetOldUpDirectionVec(mat4 camRotatedMatrix);
LOCAL vec3 GetNewUpDirectionVec(mat4 camRotatedMatrix);
LOCAL mat4 ChangeUpDirection(mat4 camRotatedMatrix, vec3 newUpDirection);


void UpdateCamera(GLint t);
void CameraMouseUpdate(GLint x, GLint y);
LOCAL mat4 CameraControl(GLint t, mat4 camRotatedMatrix, mat4 camPositionMatrix);
LOCAL mat4 AdjustCameraToHeightMap(mat4 camPositionMatrix, GLfloat height);

#endif