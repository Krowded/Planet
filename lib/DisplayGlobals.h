#ifndef DISPLAY_GLOBALS_H
#define DISPLAY_GLOBALS_H

#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "loadobj.h"
#include "Init.h"

extern const GLfloat windowWidth;
extern const GLfloat windowHeight;
extern const GLint drawDistance;
extern const GLfloat nearDrawDistance;
extern const vec4 backgroundColor;
extern const GLfloat fov;


extern Model *m, *m2, *terrainModel;
// Reference to shader program
extern GLuint terrainProgram, modelProgram;
//Texture references
extern GLuint tex1, tex2;

extern mat4 camMatrix, camBaseMatrix, projectionMatrix;

extern const GLfloat cameraHeight;

extern GLint globalTime;

extern const GLfloat radius;

extern struct planetStruct Planet;

#endif