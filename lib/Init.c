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

GLint globalTime = 0;
mat4 camMatrix, camBaseMatrix, projectionMatrix;	
Model *m, *m2, *terrainModel;
GLuint terrainProgram, modelProgram;
GLuint tex1, tex2;
TextureData terrainTexture;
mat4 TerrainModelToWorld[6];

vec3 middleOfPlanet;

void initAll()
{
	middleOfPlanet = SetVector(0,0,0);	
	initTerrain();
	initPhysics();
	initCamera();
	intiCameraControls();
	initShaders();
	initTextures();
}

void initPhysics()
{
	PhysicsInit(maxFallSpeed, cameraHeight);
}

void initCamera()
{
	vec3 cam = {0, 0, 0};
	vec3 lookAtPoint = {0, 0, -4};
	vec3 upVector = {0.0, 1.0, 0.0};

	projectionMatrix = frustum(-(windowWidth/windowHeight)*fov, (windowWidth/windowHeight)*fov, 
		                       -(windowHeight/windowWidth)*fov, (windowHeight/windowWidth)*fov, 
		                       									nearDrawDistance, drawDistance);
	camBaseMatrix = lookAt(cam.x, cam.y, cam.z,
			    lookAtPoint.x, lookAtPoint.y, lookAtPoint.z,
				upVector.x, upVector.y, upVector.z);
	camMatrix = camBaseMatrix;
}

void intiCameraControls()
{
	CameraControlsInit(standardSpeed, runSpeed, windowWidth, windowHeight);
}

void initShaders()
{
	// Load and compile shader
	terrainProgram = loadShaders("shaders/terrain.vert", "shaders/terrain.frag");
	modelProgram = loadShaders("shaders/models.vert", "shaders/models.frag");
	glUseProgram(terrainProgram);
	printError("init shader");
}

void initTextures()
{

	glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(terrainProgram, "tex"), 0); // Texture unit 0
	LoadTGATextureSimple("textures/maskros512.tga", &tex1);

	printError("init terrain");	
}

void initModels()
{
	// Load models
	m = LoadModelPlus("models/groundsphere.obj");
	m2 = LoadModelPlus("models/octagon.obj");
}


void initTerrain()
{
	// Load terrain data
	LoadTGATextureData("textures/fft-terrain.tga", &terrainTexture);

	//Generate terrain model matrix
	GLint i;
	GLfloat overlap = 0;
	GLfloat distanceToMiddleX = (terrainTexture.width*0.5)-0.5;
	GLfloat distanceToMiddleZ = (terrainTexture.height*0.5);
	GLfloat distanceToMiddleY = distanceToMiddleX - overlap;


	for (i = 0; i < 4; ++i)
	{
		TerrainModelToWorld[i] = T(-distanceToMiddleX, distanceToMiddleY, -distanceToMiddleZ);
		TerrainModelToWorld[i] = Mult( Rz(M_PI*0.5*(GLfloat)i), TerrainModelToWorld[i] );
	}
	//Last two sides

	distanceToMiddleZ = distanceToMiddleZ - 0.5;
	distanceToMiddleY = distanceToMiddleY - 0.5;
	for (i = 0; i < 2; ++i)
	{
		TerrainModelToWorld[4+i] = T(-distanceToMiddleX, distanceToMiddleY, -distanceToMiddleZ);
		TerrainModelToWorld[4+i] = Mult( Rx(M_PI*(0.5 + (GLfloat)i)), TerrainModelToWorld[4+i] );
	}
	

	//Generate terrain
	terrainModel = GenerateCubeTerrainSimple(&terrainTexture);
	//terrainModel = GenerateCubeTerrain(&terrainTexture, &terrainTexture, &terrainTexture, &terrainTexture, &terrainTexture, TerrainModelToWorld);
	GLfloat radius = distanceToMiddleY;
	terrainModel = MapCubeToFlatSphere(terrainModel, radius, terrainTexture.width, terrainTexture.height);
}