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
#include "PlanetManager.h"

#define LOCAL static

#define GLUT_ELAPSED_TIME (700)  //Because it didnt make it through the inclusion somehow?

void* chkmalloc(size_t sz);

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
void cleanUpAndExit();



#endif