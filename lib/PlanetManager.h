#ifndef PLANET_MANAGER_H
#define PLANET_MANAGER_H

#include "MicroGlut.h"
#include "Init.h"
#include "LoadTGA.h"
#include "loadobj.h"

#define LOCAL static

#define NO_SOUND 0
#define PLAY_SOUND_1 1

#define SMOOTH_PLANET 0
#define ROUGH_PLANET 1


enum planetSide {PLANET_UP, PLANET_LEFT, PLANET_DOWN, PLANET_RIGHT, PLANET_FRONT, PLANET_BACK};

struct PlanetStruct {
	mat4 ModelToWorldMatrix;
	Model* terrainModels[6]; //Each side of the cube/sphere. In order: up, left, down, right, front, back
	GLuint terrainWidth;
	GLuint terrainHeight;
	vec3 startingPosition;
	vec3 upVec;  //Must be normalized
	vec3 frontVec; //Must be normalized
	GLfloat timeOfCreation;

//These must be initialized before CreatePlanet()
	vec3 center;
	GLfloat radius;
	GLfloat orbitalSpeed;
	vec3 orbitalAxis;
	GLfloat rotationalSpeed;
	vec3 rotationalAxis;
	GLuint type;
};


void CreatePlanet(struct PlanetStruct planet, GLuint playSound);
void CreateSun(GLfloat radius);
void CreateStandardPlanet(vec3 center, GLfloat radius, GLuint playSound);

void RemoveLastPlanet();
GLuint GetCurrentPlanet();
void SetCurrentPlanet(GLuint newCurrent);
GLuint GetNumberOfPlanets();

void UpdatePlanetMovement(GLint t);

void freeAllPlanets();


#endif
