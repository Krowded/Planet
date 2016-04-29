#ifndef TERRAIN_H
#define TERRAIN_H

#include "VectorUtils3.h"
#include "LoadTGA.h"
#include "loadobj.h"

#define LOCAL static


extern struct PlanetStruct planet;

LOCAL GLfloat* GenerateTerrainVertexArray(TextureData *tex);
LOCAL GLfloat* GenerateTerrainTextureCoordinateArray(TextureData *tex);
LOCAL GLuint* GenerateTerrainIndexArray(TextureData *tex);
LOCAL GLfloat* GenerateTerrainNormalArray(TextureData *tex, GLfloat *vertexArray);

void GenerateProceduralTerrainTexture(GLint sideLength, TextureData* ter);

LOCAL int SaveAsTGA(char* filename, short int width, short int height, unsigned char* data);

Model* GenerateTerrainFromTexture(TextureData *tex);

LOCAL vec3 GetBezierPoint( vec3* points, int numPoints, float u );
LOCAL GLfloat RoundingFunction(GLfloat t);
void GenerateCubeTerrain(struct PlanetStruct* planet);

Model* GenerateCubeTerrainSimple(struct PlanetStruct planet, TextureData* terrainTextures);

Model* MapCubeToSphere(struct PlanetStruct planet, mat4* terrainTransformationMatrices, GLint i);
Model* MapCubeToFlatSphere(struct  PlanetStruct planet, mat4* terrainTransformationMatrices, GLint i);


GLfloat GetTerrainHeight(vec3, Model*, TextureData);

#endif
