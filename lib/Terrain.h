#ifndef TERRAIN_H
#define TERRAIN_H

#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"
#include "math.h"

typedef struct offset {
	GLfloat x;
	GLfloat y;
	GLfloat z;
} offset;

extern struct offset TerrainOffset;


void SetTerrainOffset(GLfloat x, GLfloat y, GLfloat z);

//These four should maybe be local
GLfloat* GenerateTerrainVertexArray(TextureData *tex);
GLfloat* GenerateTerrainTextureCoordinateArray(TextureData *tex);
GLuint* GenerateTerrainIndexArray(TextureData *tex);
GLfloat* GenerateTerrainNormalArray(TextureData *tex, GLfloat *vertexArray);


Model* GenerateTerrain(TextureData*);

Model* GenerateCubeTerrain(TextureData *mainTexture, 
						   TextureData *leftConnectingTexture, 
						   TextureData *rightConnectingTexture, 
	                       TextureData *upConnectingTexture, 
	                       TextureData *downConnectingTexture,
	                       mat4 modelMatrices[6]);

Model* GenerateCubeTerrainSimple(TextureData *mainTexture);

Model* MapCubeToSphere(Model* cubeModel, GLfloat radius, GLint arrayWidth, GLint arrayHeight);
Model* MapCubeToFlatSphere(Model* cubeModel, GLfloat radius, GLint arrayWidth, GLint arrayHeight);


GLfloat GetTerrainHeight(vec3, Model*, TextureData);

#endif
