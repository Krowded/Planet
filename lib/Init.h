#ifndef INIT_H
#define INIT_H

#include "DisplayGlobals.h"
#include "ControlGlobals.h"
#include "PhysicsGlobals.h"
#include "TerrainGlobals.h"

#include "loadobj.h"
#include "LoadTGA.h"
#include "VectorUtils3.h"
#include "Terrain.h"

#define LOCAL static

void* chkmalloc(size_t sz);

//Because declaration order matters....
struct planetStruct {
	TextureData* terrainTexture[6]; //Order: Up, left, bottom, right, front, back
	mat4 terrainModelToWorld[6];	//Same order as terrainTexture
	Model* terrainModels[6];
	vec3 center;
	vec3 upVec;  //Must be normalized
	vec3 frontVec; //Must be normalized
	GLfloat radius;
};

extern struct planetStruct Planet;

void InitAll();
LOCAL void InitPhysics();
LOCAL void InitCamera();
LOCAL void InitCameraControls();
LOCAL void InitShaders();
LOCAL void InitTextures();
LOCAL void InitTerrain();
void InitModels();
void InitWindow(GLint width, GLint height);
void SetCurrentPlanet(GLuint newCurrent);
void cleanUpAndExit();

#endif