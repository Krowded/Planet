#ifndef TERRAIN_H
#define TERRAIN_H

#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"
#include "math.h"
#include "Init.h"
#include "TerrainGlobals.h"

#define LOCAL static


extern struct planetStruct planet;

LOCAL GLfloat* GenerateTerrainVertexArray(TextureData *tex);
LOCAL GLfloat* GenerateTerrainTextureCoordinateArray(TextureData *tex);
LOCAL GLuint* GenerateTerrainIndexArray(TextureData *tex);
LOCAL GLfloat* GenerateTerrainNormalArray(TextureData *tex, GLfloat *vertexArray);

void GenerateProceduralTerrainTexture(GLint sideLength, TextureData* ter);

LOCAL int SaveAsTGA(char* filename, short int width, short int height, unsigned char* data);

Model* GenerateTerrainFromTexture(TextureData *tex);

LOCAL vec3 GetBezierPoint( vec3* points, int numPoints, float u );

void GenerateCubeTerrain(struct planetStruct* planet);

Model* GenerateCubeTerrainSimple(struct planetStruct* planet);

Model* MapCubeToSphere(struct planetStruct* planet, GLint i);
Model* MapCubeToFlatSphere(struct  planetStruct planet, GLint i);


GLfloat GetTerrainHeight(vec3, Model*, TextureData);

#endif
