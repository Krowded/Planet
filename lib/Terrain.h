#ifndef TERRAIN_H
#define TERRAIN_H

#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"
#include "math.h"
#include "Init.h"



#define LOCAL static

extern struct planetStruct planet;

LOCAL GLfloat* GenerateTerrainVertexArray(TextureData *tex);
LOCAL GLfloat* GenerateTerrainTextureCoordinateArray(TextureData *tex);
LOCAL GLuint* GenerateTerrainIndexArray(TextureData *tex);
LOCAL GLfloat* GenerateTerrainNormalArray(TextureData *tex, GLfloat *vertexArray);

void GenerateProceduralTerrainTexture(GLint sideLength, TextureData* ter);

LOCAL int SaveAsTGA(char* filename, short int width, short int height, unsigned char* data);

Model* GenerateTerrainFromTexture(TextureData *tex);

Model* GenerateCubeTerrain(TextureData *mainTexture, 
						   TextureData *leftConnectingTexture, 
						   TextureData *rightConnectingTexture, 
	                       TextureData *upConnectingTexture, 
	                       TextureData *downConnectingTexture,
	                       mat4 modelMatrices[6]);

Model* GenerateCubeTerrainSimple(struct planetStruct *planet);

Model* MapCubeToSphere(Model* cubeModel, GLfloat radius, GLint arrayWidth, GLint arrayHeight);
Model* MapCubeToFlatSphere(Model* cubeModel, GLfloat radius, GLint arrayWidth, GLint arrayHeight);


GLfloat GetTerrainHeight(vec3, Model*, TextureData);

#endif
