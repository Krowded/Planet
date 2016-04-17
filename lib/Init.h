#ifndef Init_H
#define Init_H

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

extern const GLfloat terrainScale;

extern mat4 camMatrix, camBaseMatrix, projectionMatrix;	

extern Model *m, *m2, *terrainModel;
// Reference to shader program
extern GLuint terrainProgram, modelProgram;
//Texture references
extern GLuint tex1, tex2;

extern GLint globalTime;

extern struct planetStruct Planet;

extern vec3 middleOfPlanet;


void InitAll();
void InitPhysics();
void InitCamera();
void InitCameraControls();
void InitShaders();
void InitTextures();
void InitModels();
void InitTerrain();

#endif