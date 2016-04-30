#ifndef INIT_H
#define INIT_H

#include "VectorUtils3.h"
#include "MicroGlut.h"
#include "PlanetManager.h"

#define LOCAL static

#define GLUT_ELAPSED_TIME (700)  //Because it didnt make it through the inclusion somehow?

void* chkmalloc(size_t sz);

extern struct planetStruct Planet;

void InitAll();

void InitModels();
void InitWindow(GLint width, GLint height);
void cleanUpAndExit();



#endif