#ifndef PLANET_MANAGER_H
#define PLANET_MANAGER_H

#include "MicroGlut.h"
#include "Init.h"
#include "LoadTGA.h"
#include "loadobj.h"

#define LOCAL static

enum planetSide {PLANET_UP, PLANET_LEFT, PLANET_DOWN, PLANET_RIGHT, PLANET_FRONT, PLANET_BACK};

struct PlanetStruct {
	mat4 ModelToWorldMatrix;
	Model* terrainModels[6]; //Each side of the cube/sphere. In order: up, left, down, right, front, back
	GLuint terrainWidth;
	GLuint terrainHeight;
	vec3 startingPosition;
	vec3 center;
	vec3 upVec;  //Must be normalized
	vec3 frontVec; //Must be normalized
	GLfloat radius;
	GLfloat orbitalSpeed;
	vec3 orbitalAxis;
	GLfloat rotationalSpeed;
	vec3 rotationalAxis;
	GLfloat timeOfCreation;
};


void CreatePlanet(vec3 center, GLfloat radius, vec3 upVec, vec3 frontVec, GLfloat orbitalSpeed, vec3 orbitalAxis, GLfloat rotationalSpeed, vec3 rotationalAxis);
void CreateSun(GLfloat radius);

void RemoveLastPlanet();
void SetCurrentPlanet(GLuint newCurrent);
void UpdatePlanetMovement(GLint t);


void freePlanet(struct PlanetStruct planet);


#endif
