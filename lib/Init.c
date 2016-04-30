
#include "DisplayGlobals.h"
#include "ControlGlobals.h"
#include "PhysicsGlobals.h"
#include "TerrainGlobals.h"

#include "Init.h"
#include "Terrain.h"
#include "loadobj.h"
#include "LoadTGA.h"

LOCAL void InitMusic();
LOCAL void InitCamera();
LOCAL void InitShaders();
LOCAL void InitTextures();
LOCAL void InitTerrain();

void* chkmalloc(size_t sz) 
{
	void *ret = malloc(sz);
	if(ret == NULL)
		{
			fprintf(stderr, "Out of memory\n");
			exit(EXIT_FAILURE);
		}
	return ret;
}

//Define globals
const vec4 backgroundColor = {1,1,1,0};

GLfloat windowWidth = 1024.0;
GLfloat windowHeight = 576.0;
GLint drawDistance = 10000.0;
GLfloat nearDrawDistance = 0.2;
GLfloat fov = 90;

vec3 startingUp = {0, 1, 0};
vec3 startingPosition = {0, 0, 200};

GLfloat maxFallSpeed = 5;
GLfloat maxRotationSpeed = 0.001;
GLfloat cameraHeight = 5;
GLfloat standardSpeed = 0.02;
GLfloat runSpeed = 1.0;
GLint NewGravity = GL_FALSE;

GLfloat mouseSensitivity = 0.002;

GLfloat maxHeight = 100;

GLfloat terrainScale = 20.0;
GLuint roundingDistanceFromEdge = 30;
GLfloat simpleInterpolationHeight = 10;

GLint globalTime = 0;
mat4 camMatrix, camBaseMatrix, projectionMatrix;	
Model *m, *m2, *terrainModel;
GLuint terrainProgram, modelProgram;
GLuint tex1, tex2;


GLfloat maxAngleOfTerrain = 0.78539816339;

struct PlanetStruct* planetsList;

GLfloat orbitSpeed = 0.001;

const char* soundfile = "audio/test.mp3";

void InitAll()
{
	InitWindow(windowWidth, windowHeight);
	InitMusic();
	InitTerrain();
	InitCamera();
	InitShaders();
	InitTextures();

}

LOCAL void InitMusic()
{
	PlayAudioFile(soundfile);
}

LOCAL void InitCamera()
{
	vec3 cam = startingPosition;
	vec3 lookAtPoint = startingPosition;
	lookAtPoint.z -= 10;
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
	//Initialize

	CreateSun(128);

	//Create a starting number of planets, i.e. 1;
	vec3 center = SetVector(0, 0, -500);
	GLfloat radius = 256.f*0.5f;
	CreateStandardPlanet(center, radius, NO_SOUND);
}



void InitWindow(GLint width, GLint height)
{
	windowWidth = width;
	windowHeight = height;
	glViewport(0,0,windowWidth, windowHeight);
}




void cleanUpAndExit()
{
	freeAllPlanets();
	KillAllAudio();

	exit(0);
}