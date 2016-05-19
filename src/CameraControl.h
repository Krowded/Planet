#ifndef CAMERA_CONTROL_H
#define CAMERA_CONTROL_H

#include "MicroGlut.h"
#include "VectorUtils3.h"
#include "PlanetManager.h"

#define LOCAL static

vec3 GetCurrentCameraPosition(mat4 camPositionMatrix);
mat4 SetCameraPosition(mat4 camPositionMatrix, vec3 position);

void UpdateCamera(GLint t, struct PlanetStruct planet);
void CameraMouseUpdate(GLint x, GLint y);

#endif