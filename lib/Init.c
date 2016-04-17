#include "Init.h"

//Define globals

const GLfloat windowWidth = 1024.0;
const GLfloat windowHeight = 576.0;
const GLint drawDistance = 10000.0;
const GLfloat nearDrawDistance = 0.2;
const vec4 backgroundColor = {1,1,1,0};
const GLfloat fov = 0.2;
const GLfloat maxFallSpeed = 10;
const GLfloat cameraHeight = 5;
const GLfloat standardSpeed = 0.02;
const GLfloat runSpeed = 1.0;

const GLfloat terrainScale = 30.0;

GLint globalTime = 0;
mat4 camMatrix, camBaseMatrix, projectionMatrix;	
Model *m, *m2, *terrainModel;
GLuint terrainProgram, modelProgram;
GLuint tex1, tex2;

struct planetStruct Planet;
vec3 middleOfPlanet ;

void InitAll()
{	
	InitTerrain();
	InitPhysics();
	InitCamera();
	InitCameraControls();
	InitShaders();
	InitTextures();

	middleOfPlanet = SetVector(Planet.terrainTexture[0].width/2, 0, Planet.terrainTexture[0].height/2);
}

void InitPhysics()
{
	PhysicsInit(maxFallSpeed, cameraHeight);
}

void InitCamera()
{
	vec3 cam = {0, 200, 0};
	vec3 lookAtPoint = {0, 200, -4};
	vec3 upVector = {0.0, 1.0, 0.0};

	projectionMatrix = frustum(-(windowWidth/windowHeight)*fov, (windowWidth/windowHeight)*fov, 
		                       -(windowHeight/windowWidth)*fov, (windowHeight/windowWidth)*fov, 
		                       									nearDrawDistance, drawDistance);
	camBaseMatrix = lookAt(cam.x, cam.y, cam.z,
			    lookAtPoint.x, lookAtPoint.y, lookAtPoint.z,
				upVector.x, upVector.y, upVector.z);
	camMatrix = camBaseMatrix;
}

void InitCameraControls()
{
	CameraControlsInit(standardSpeed, runSpeed, windowWidth, windowHeight);
}

void InitShaders()
{
	// Load and compile shader
	terrainProgram = loadShaders("shaders/terrain.vert", "shaders/terrain.frag");
	modelProgram = loadShaders("shaders/models.vert", "shaders/models.frag");
	glUseProgram(terrainProgram);
	printError("Init shader");
}

void InitTextures()
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


void InitTerrain()
{
	// Load terrain data
	GLint i;
	for (i = 0; i < 6; i++)
		LoadTGATextureData("textures/fft-terrain.tga", &(Planet.terrainTexture[i]));

	//Generate terrain model matrix
	GLfloat overlap = 0;
	GLfloat distanceToMiddleX = (Planet.terrainTexture[0].width*0.5)-0.5;
	GLfloat distanceToMiddleZ = (Planet.terrainTexture[0].height*0.5);
	GLfloat distanceToMiddleY = distanceToMiddleX - overlap;


	for (i = 0; i < 4; ++i)
	{
		Planet.TerrainModelToWorld[i] = T(-distanceToMiddleX, distanceToMiddleY, -distanceToMiddleZ);
		Planet.TerrainModelToWorld[i] = Mult( Rz(M_PI*0.5*(GLfloat)i), Planet.TerrainModelToWorld[i] );
	}
	//Last two sides

	distanceToMiddleZ = distanceToMiddleZ - 0.5;
	distanceToMiddleY = distanceToMiddleY - 0.5;
	for (i = 0; i < 2; ++i)
	{
		Planet.TerrainModelToWorld[4+i] = T(-distanceToMiddleX, distanceToMiddleY, -distanceToMiddleZ);
		Planet.TerrainModelToWorld[4+i] = Mult( Rx(M_PI*(0.5 + (GLfloat)i)), Planet.TerrainModelToWorld[4+i] );
	}
	

	//Generate terrain
	terrainModel = GenerateCubeTerrainSimple(&Planet);
	//terrainModel = GenerateCubeTerrain(&Planet);
	GLfloat radius = distanceToMiddleY;
	terrainModel = MapCubeToSphere(terrainModel, radius, Planet.terrainTexture[0].width, Planet.terrainTexture[0].height);
}