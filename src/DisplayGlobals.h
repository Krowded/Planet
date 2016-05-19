#ifndef DISPLAY_GLOBALS_H
#define DISPLAY_GLOBALS_H

#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "loadobj.h"
#include "Init.h"


extern const vec4 backgroundColor;

extern GLfloat windowWidth;
extern GLfloat windowHeight;
extern GLint drawDistance;
extern GLfloat nearDrawDistance;
extern GLfloat fov;

// Reference to shader program
extern GLuint terrainProgram, modelProgram, sunProgram, skyboxProgram;
//Texture references
extern GLuint tex1, tex2, sunTexture, skyboxTopTexture, skyboxBottomTexture, 
skyboxLeftTexture, skyboxRightTexture, skyboxFrontTexture, skyboxBackTexture;

extern Model *skyTop, *skyBottom, *skyLeft, *skyRight, *skyFront, *skyBack;

extern mat4 camMatrix, camBaseMatrix, projectionMatrix;

extern GLfloat cameraHeight;

extern struct PlanetStruct* planetsList;

#endif