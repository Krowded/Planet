#ifndef INIT_H
#define INIT_H

#include "loadobj.h"
#include "LoadTGA.h"
#include "VectorUtils3.h"

#include "CameraControl.h"
#include "Physics.h"

//Because declaration order matters....
struct planetStruct {
	TextureData terrainTexture[6];
	mat4 TerrainModelToWorld[6];	
};

#include "Terrain.h"


//Declare globals

extern const GLfloat windowWidth;
extern const GLfloat windowHeight;
extern const GLint drawDistance;
extern const GLfloat nearDrawDistance;
extern const vec4 backgroundColor;
extern const GLfloat fov;
extern const GLfloat maxFallSpeed;
extern const GLfloat cameraHeight;
extern const GLfloat standardSpeed;
extern const GLfloat runSpeed;

extern mat4 camMatrix, camBaseMatrix, projectionMatrix;	

extern Model *m, *m2, *terrainModel;
// Reference to shader program
extern GLuint terrainProgram, modelProgram;
//Texture references
extern GLuint tex1, tex2;
extern TextureData terrainTexture; // Terrain, stored as a texture

extern GLint globalTime;

//Terrain model matrices
extern mat4 TerrainModelToWorld[6];

extern struct planetStruct Planet;


void initAll();
void initPhysics();
void initCamera();
void intiCameraControls();
void initShaders();
void initTextures();
void initModels();
void initTerrain();

#endif