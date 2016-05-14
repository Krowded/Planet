#include "Skybox.h"
#include "MicroGlut.h"
#include "loadobj.h"
#include "LoadTGA.h"
#include "DisplayGlobals.h"

static  GLfloat skyboxTopVertices[] = {
    -1.0f,1.0f,-1.0f,
    -1.0f,1.0f,1.0f,
     1.0f,1.0f,1.0f,
     1.0f,1.0f,-1.0f
};
static  GLuint skyboxTopIndices[] = { 0, 1, 2, 0, 2, 3 };

static  GLfloat skyboxTopTexCoords[] = { 
    0, 0, 
    0, 1, 
    1, 1,
    1, 0 };


static  GLfloat skyboxBottomVertices[] = {
     1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f, 1.0f
};
static  GLuint skyboxBottomIndices[] = { 0, 1, 2, 0, 3, 1 };

static  GLfloat skyboxBottomTexCoords[] = { 
    1, 0, 
    0, 1, 
    1, 1,
    0, 0 };


static  GLfloat skyboxLeftVertices[] = {
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f
};
static  GLuint skyboxLeftIndices[] = { 0, 1, 2, 0, 2, 3 };

static  GLfloat skyboxLeftTexCoords[] = { 
    0, 1, 
    1, 1, 
    1, 0,
    0, 0 };


static  GLfloat skyboxRightVertices[] = {
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, 1.0f
};
static  GLuint skyboxRightIndices[] = { 0, 1, 2, 0, 3, 1};

static  GLfloat skyboxRightTexCoords[] = { 
    1, 1, 
    0, 0, 
    1, 0,
    0, 1 };


static  GLfloat skyboxFrontVertices[] = {
    -1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f
};
static  GLuint skyboxFrontIndices[] = { 0, 1, 2, 0, 3, 1 };

static  GLfloat skyboxFrontTexCoords[] = { 
    0, 1, 
    1, 0, 
    0, 0,
    1, 1 };


static  GLfloat skyboxBackVertices[] = {
    -1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f
};
static GLuint skyboxBackIndices[] = { 0, 1, 2, 0, 3, 1 };

static  GLfloat skyboxBackTexCoords[] = { 
    1, 1, 
    0, 0, 
    1, 0,
    0, 1 };


Model *skyTop, *skyBottom, *skyLeft, *skyRight, *skyFront, *skyBack;

void InitSkybox()
{
    skyTop = LoadDataToModel( 
            skyboxTopVertices, 
            NULL, 
            skyboxTopTexCoords,
            NULL,
            skyboxTopIndices,
            3*2,
            6);

    skyBottom = LoadDataToModel( 
            skyboxBottomVertices, 
            NULL, 
            skyboxBottomTexCoords,
            NULL,
            skyboxBottomIndices,
            3*2,
            6);

    skyLeft = LoadDataToModel( 
            skyboxLeftVertices, 
            NULL, 
            skyboxLeftTexCoords,
            NULL,
            skyboxLeftIndices,
            3*2,
            6);
 
    skyRight = LoadDataToModel( 
            skyboxRightVertices, 
            NULL, 
            skyboxRightTexCoords,
            NULL,
            skyboxRightIndices,
            3*2,
            6);

    skyFront = LoadDataToModel( 
            skyboxFrontVertices, 
            NULL, 
            skyboxFrontTexCoords,
            NULL,
            skyboxFrontIndices,
            3*2,
            6);
    
    skyBack = LoadDataToModel( 
            skyboxBackVertices, 
            NULL, 
            skyboxBackTexCoords,
            NULL,
            skyboxBackIndices,
            3*2,
            6);
}
