#ifndef INIT_H
#define INIT_H

#include "DisplayGlobals.h"
#include "ControlGlobals.h"
#include "PhysicsGlobals.h"

#include "loadobj.h"
#include "LoadTGA.h"
#include "VectorUtils3.h"
#include "Terrain.h"

#define LOCAL static

//Because declaration order matters....
struct planetStruct {
	TextureData terrainTexture[6];
	mat4 TerrainModelToWorld[6];	
};




//Declare globals
extern const GLfloat terrainScale;

extern TextureData terrainTexture; // Terrain, stored as a texture

extern struct planetStruct Planet;

extern vec3 middleOfPlanet;

void InitAll();
LOCAL void InitPhysics();
LOCAL void InitCamera();
LOCAL void InitCameraControls();
LOCAL void InitShaders();
LOCAL void InitTextures();
void InitModels();
LOCAL void InitTerrain();
void InitWindow(GLint width, GLint height);
#endif