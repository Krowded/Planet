#ifndef PHYSICS_H
#define PHYSICS_H

#include "VectorUtils3.h"
#include "math.h"
#include "Init.h"
#include "PhysicsGlobals.h"
	
//Bool if TGA not included
#ifndef __TGA_LOADER__

	typedef GLint bool;
	#define false 0
	#define true !false

#endif 

void PhysicsInit(GLfloat maxFallSpeed, GLfloat cameraHeight);
void SetGravity(bool isGravityOn);
void SetFallSpeed(GLfloat maxFallSpeed);
void SetCameraHeight(GLfloat cameraHeight);
bool IsGravityOn();

mat4 AdjustModelToHeightMap(mat4 ModelToWorldMatrix, struct planetStruct planet);

vec3 GetCurrentPosition(mat4 ModelToWorldMatrix);

#endif