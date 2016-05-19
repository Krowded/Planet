#ifndef PLANET_MANAGER_H
#define PLANET_MANAGER_H

#include "MicroGlut.h"
#include "Init.h"
#include "LoadTGA.h"
#include "loadobj.h"

#define LOCAL static

enum sounds {NO_SOUND, PLAY_SOUND_1};
enum planetShape {SMOOTH_PLANET, ROUGH_PLANET, CUBE_PLANET};
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
	GLfloat textureScale;

//These must be initialized before CreatePlanet()
	vec3 center;
	GLfloat radius;
	GLfloat orbitalSpeed;
	vec3 orbitalAxis;
	GLfloat rotationalSpeed;
	vec3 rotationalAxis;
	enum planetShape type;
};


void CreatePlanet(struct PlanetStruct planet, GLuint playSound);
void CreateSun(GLfloat radius, enum planetShape shape);
void CreateStandardPlanet(vec3 center, GLfloat radius, enum planetShape shape, GLuint playSound);

void RemoveLastPlanet();
GLuint GetCurrentPlanet();
void SetCurrentPlanet(GLuint newCurrent);
GLuint GetNumberOfPlanets();

void UpdatePlanetMovement(GLint t);

void deleteAllPlanets();


#endif
