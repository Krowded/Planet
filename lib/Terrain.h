#ifndef TERRAIN_H
#define TERRAIN_H

#include "VectorUtils3.h"
#include "LoadTGA.h"
#include "loadobj.h"

#define LOCAL static


extern struct PlanetStruct planet;


void GenerateProceduralTerrainTexture(GLint sideLength, TextureData* ter);

Model* GenerateTerrainFromTexture(TextureData *tex, GLfloat textureScale);

void GenerateCubeTerrain(struct PlanetStruct* planet);

Model* GenerateCubeTerrainSimple(TextureData* terrainTextures, GLfloat textureScale);

Model* CreateCube(struct PlanetStruct planet, mat4* terrainTransformationMatrices, TextureData* terrainTexture, GLuint i);
Model* MapCubeToSphere(struct PlanetStruct planet, mat4* terrainTransformationMatrices, TextureData* terrainTexture, GLuint i);
Model* MapCubeToFlatSphere(struct  PlanetStruct planet, mat4* terrainTransformationMatrices, TextureData* terrainTexture, GLuint i);


GLfloat GetTerrainHeight(vec3, Model*, TextureData);

#endif
