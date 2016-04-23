#include "Init.h"

//Define globals
const vec4 backgroundColor = {1,1,1,0};

GLfloat windowWidth = 1024.0;
GLfloat windowHeight = 576.0;
GLint drawDistance = 10000.0;
GLfloat nearDrawDistance = 0.2;
GLfloat fov = 90;

vec3 startingUp = {0, 1, 0};

GLfloat maxFallSpeed = 5;
GLfloat maxRotationSpeed = 0.05;
GLfloat cameraHeight = 5;
GLfloat standardSpeed = 0.02;
GLfloat runSpeed = 1.0;

GLfloat mouseSensitivity = 0.002;

GLfloat maxHeight = 100;;
const GLfloat terrainScale = 30.0;

GLint globalTime = 0;
mat4 camMatrix, camBaseMatrix, projectionMatrix;	
Model *m, *m2, *terrainModel;
GLuint terrainProgram, modelProgram;
GLuint tex1, tex2;
TextureData terrainTexture;

const GLfloat radius = 128;

struct planetStruct Planet;
vec3 middleOfPlanet;

void InitAll()
{
	InitWindow(windowWidth, windowHeight);
	middleOfPlanet = SetVector(0,0,0);
	Planet.center = SetVector(0,0,0);
	Planet.radius = radius;
	Planet.upvec = SetVector(0, 1, 0);
	Planet.frontVec = SetVector(0, 0, 1);
	InitTerrain();
	InitCamera();
	InitShaders();
	InitTextures();
}	

LOCAL void InitCamera()
{
	vec3 cam = {0, 100, 0};
	vec3 lookAtPoint = cam;
	lookAtPoint.z -= 4.0;
	vec3 upVector = {0.0, 1.0, 0.0};

	projectionMatrix = perspective(fov, windowWidth/windowHeight, nearDrawDistance, drawDistance);

	camBaseMatrix = lookAt(cam.x, cam.y, cam.z,
			    lookAtPoint.x, lookAtPoint.y, lookAtPoint.z,
				upVector.x, upVector.y, upVector.z);
	camMatrix = camBaseMatrix;
}

LOCAL void InitShaders()
{
	// Load and compile shader
	terrainProgram = loadShaders("shaders/terrain.vert", "shaders/terrain.frag");
	modelProgram = loadShaders("shaders/models.vert", "shaders/models.frag");
	glUseProgram(terrainProgram);
	printError("Init shader");
}

LOCAL void InitTextures()
{
	glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(terrainProgram, "tex"), 0); // Texture unit 0
	LoadTGATextureSimple("textures/maskros512.tga", &tex1);

	printError("Init terrain");	
}

void InitModels()
{
	// Load models
	m = LoadModelPlus("models/groundsphere.obj");
	m2 = LoadModelPlus("models/octagon.obj");
}


LOCAL void InitTerrain()
{
	// Load terrain data
	GLint i;
	for (i = 0; i < 6; i++)
		LoadTGATextureData("textures/fft-terrain.tga", &(Planet.terrainTexture[i]));

	//Generate terrain model matrix
	GLfloat overlap = 0;
	GLfloat distanceToMiddleX = (Planet.terrainTexture[0].width*0.5);
	GLfloat distanceToMiddleZ = (Planet.terrainTexture[0].height*0.5);
	GLfloat distanceToMiddleY = distanceToMiddleX - overlap;


	for (i = 0; i < 4; ++i)
	{
		Planet.TerrainModelToWorld[i] = T(-distanceToMiddleX, distanceToMiddleY, -distanceToMiddleZ);
		Planet.TerrainModelToWorld[i] = Mult( Rz(M_PI*0.5*(GLfloat)i), Planet.TerrainModelToWorld[i] );
	}
	//Last two sides

	//distanceToMiddleZ = distanceToMiddleZ - 0.5;
	//distanceToMiddleY = distanceToMiddleY - 0.5;
	for (i = 0; i < 2; ++i)
	{
		Planet.TerrainModelToWorld[4+i] = T(-distanceToMiddleX, distanceToMiddleY, -distanceToMiddleZ);
		Planet.TerrainModelToWorld[4+i] = Mult( Rx(M_PI*(0.5 + (GLfloat)i)), Planet.TerrainModelToWorld[4+i] );
	}

	TextureData* tex = malloc(sizeof(TextureData));
	GenerateProceduralTerrainTexture(256, tex);
	Planet.terrainTexture[0] = *tex;
	//Generate terrain
	terrainModel = GenerateCubeTerrainSimple(&Planet);
	//terrainModel = GenerateTerrainFromTexture(tex);
	//terrainModel = GenerateCubeTerrain(&Planet);
	GLfloat radius = distanceToMiddleY;
	//terrainModel = MapCubeToFlatSphere(terrainModel, radius, Planet.terrainTexture[0].width, Planet.terrainTexture[0].height);
}


void InitWindow(GLint width, GLint height)
{
	windowWidth = width;
	windowHeight = height;
	glViewport(0,0,windowWidth, windowHeight);
}