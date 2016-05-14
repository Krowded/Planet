#ifndef TERRAIN_H
#define TERRAIN_H

#include "VectorUtils3.h"
#include "LoadTGA.h"
#include "loadobj.h"

#define LOCAL static


extern struct PlanetStruct planet;


void GenerateProceduralTerrainTexture(GLint sideLength, TextureData* ter);

Model* GenerateTerrainFromTexture(TextureData *tex, GLfloat textureScale);

Model* GenerateTerrainModelSimple(TextureData* terrainTextures, GLfloat textureScale);

Model* MapToCube(struct PlanetStruct planet, mat4* terrainTransformationMatrices, TextureData* terrainTexture, GLuint i);
Model* MapToSphere(struct PlanetStruct planet, mat4* terrainTransformationMatrices, TextureData* terrainTexture, GLuint i);
Model* MapToFlatSphere(struct  PlanetStruct planet, mat4* terrainTransformationMatrices, TextureData* terrainTexture, GLuint i);


GLfloat GetTerrainHeight(vec3, Model*, TextureData);

#endif
