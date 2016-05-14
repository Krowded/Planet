
#include "DisplayGlobals.h"
#include "ControlGlobals.h"
#include "PhysicsGlobals.h"
#include "TerrainGlobals.h"

#include "Init.h"
#include "Terrain.h"
#include "loadobj.h"
#include "LoadTGA.h"
#include "PlayAudioFile.h"

#include "Skybox.h"

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
const vec4 backgroundColor = {0,0,0,0};

GLfloat windowWidth = 1024.0;
GLfloat windowHeight = 576.0;
GLint drawDistance = 10000.0;
GLfloat nearDrawDistance = 0.2;
GLfloat fov = 90;

vec3 startingPosition = {0, 0, 200};

GLfloat maxFallSpeed = 5;
GLfloat maxRotationSpeed = 0.005;
GLfloat cameraHeight = 5;
GLfloat standardSpeed = 0.02;
GLfloat runSpeed = 1.0;
GLint NewGravity = GL_FALSE;

GLfloat mouseSensitivity = 0.002;

GLfloat maxHeight = 100;

GLfloat terrainScale = 20.0;
GLuint roundingDistanceFromEdge = 30;
GLfloat simpleInterpolationHeight = 10;

mat4 camMatrix, camBaseMatrix, projectionMatrix;	
GLuint terrainProgram, modelProgram, sunProgram, skyboxProgram, skyboxTopTexture, skyboxBottomTexture, 
skyboxLeftTexture, skyboxRightTexture, skyboxFrontTexture, skyboxBackTexture;
GLuint tex1, tex2, sunTexture;


GLfloat maxAngleOfTerrain = 0.78539816339;

struct PlanetStruct* planetsList;

GLfloat orbitSpeed = 0.001;

char* soundfile = "audio/test.mp3asfasfasf";

void InitAll()
{
	InitWindow(windowWidth, windowHeight);
	InitMusic();
	InitTerrain();
	InitCamera();
	InitShaders();
	InitTextures();

	InitSkybox();

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
	sunProgram = loadShaders("shaders/sun.vert", "shaders/sun.frag");
	skyboxProgram = loadShaders("shaders/skybox.vert", "shaders/skybox.frag");
	printError("Init shader");
}

LOCAL void InitTextures()
{
	LoadTGATextureSimple("textures/maskros512.tga", &tex1);
	LoadTGATextureSimple("textures/sun.tga", &sunTexture);

	LoadTGATextureSimple("textures/bluepurplenebula4096_top3.tga", &skyboxTopTexture);
	LoadTGATextureSimple("textures/bluepurplenebula4096_bottom4.tga", &skyboxBottomTexture);
	LoadTGATextureSimple("textures/bluepurplenebula4096_left2.tga", &skyboxLeftTexture);
	LoadTGATextureSimple("textures/bluepurplenebula4096_right1.tga", &skyboxRightTexture);
	LoadTGATextureSimple("textures/bluepurplenebula4096_front5.tga", &skyboxFrontTexture);
	LoadTGATextureSimple("textures/bluepurplenebula4096_back6.tga", &skyboxBackTexture);

/*
	LoadTGATextureSimple("textures/simpleskybox2048_top3.tga", &skyboxTopTexture);
	LoadTGATextureSimple("textures/simpleskybox2048_bottom4.tga", &skyboxBottomTexture);
	LoadTGATextureSimple("textures/simpleskybox2048_left2.tga", &skyboxLeftTexture);
	LoadTGATextureSimple("textures/simpleskybox2048_right1.tga", &skyboxRightTexture);
	LoadTGATextureSimple("textures/simpleskybox2048_front5.tga", &skyboxFrontTexture);
	LoadTGATextureSimple("textures/simpleskybox2048_back6.tga", &skyboxBackTexture);
*/
	printError("Init terrain");	
}

LOCAL void InitTerrain()
{
	//Initialize

	//Make a sun at the center
	CreateSun(128, SMOOTH_PLANET);

	//Create a starting number of planets, i.e. 1;
	vec3 center = SetVector(0, 0, -500);
	GLfloat radius = 256.f*0.5f;
	CreateStandardPlanet(center, radius, ROUGH_PLANET, NO_SOUND);
}



void InitWindow(GLint width, GLint height)
{
	windowWidth = width;
	windowHeight = height;
	glViewport(0,0,windowWidth, windowHeight);
}




void cleanUpAndExit()
{
	deleteAllPlanets();
	KillAllAudio();

	exit(0);
}
