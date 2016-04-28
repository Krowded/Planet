#include "Init.h"

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
vec3 startingPosition = {0, 100, 0};

GLfloat maxFallSpeed = 5;
GLfloat maxRotationSpeed = 0.05;
GLfloat cameraHeight = 5;
GLfloat standardSpeed = 0.02;
GLfloat runSpeed = 1.0;

GLfloat mouseSensitivity = 0.002;

GLfloat maxHeight = 100;;

GLfloat terrainScale = 20.0;
GLint roundingDistanceFromEdge = 30;
GLfloat simpleInterpolationHeight = 10;

GLint globalTime = 0;
mat4 camMatrix, camBaseMatrix, projectionMatrix;	
Model *m, *m2, *terrainModel;
GLuint terrainProgram, modelProgram;
GLuint tex1, tex2;


GLfloat maxAngleOfTerrain = 0.78539816339;

struct planetStruct* planetsList;
GLint numberOfPlanets;
GLint currentPlanet = 0;


void InitAll()
{
	InitWindow(windowWidth, windowHeight);
	InitTerrain();
	InitCamera();
	InitShaders();
	InitTextures();

}	

LOCAL void InitCamera()
{
	vec3 cam = startingPosition;
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
	numberOfPlanets = 0;
	planetsList = malloc(sizeof(struct planetStruct)*numberOfPlanets);

	GLint j;
	for(j = 0; j < 2; j++)
	{
		vec3 center = SetVector(cos(j*2*M_PI/5)*500, 0, sin(j*2*M_PI/5)*500);
		GLfloat radius = 256.0f*0.5f;
		vec3 upVec = SetVector(0, 1, 0);
		vec3 frontVec = SetVector(0, 0, 1);
		CreatePlanet(center, radius, upVec, frontVec);
	}
	
}

void CreatePlanet(vec3 center, GLfloat radius, vec3 upVec, vec3 frontVec)
{
	numberOfPlanets++;
	planetsList = realloc(planetsList, sizeof(struct planetStruct)*numberOfPlanets);

	struct planetStruct Planet;// = *(struct planetStruct*)chkmalloc(sizeof(struct planetStruct));
	Planet.center = center;
	Planet.radius = radius;
	Planet.upVec = upVec;
	Planet.frontVec = frontVec;


	GLint i;	
	for(i = 0; i < 6; i++)
	{
		Planet.terrainTexture[i] = chkmalloc(sizeof(TextureData));
	}

	// Load terrain data
	for (i = 0; i < 6; i++)
		LoadTGATextureData("textures/fft-terrain.tga", Planet.terrainTexture[i]);

	//Generate terrain model matrix
	GLfloat distanceToMiddleX = ((GLfloat)Planet.terrainTexture[0]->width*0.5f);
	GLfloat distanceToMiddleZ = ((GLfloat)Planet.terrainTexture[0]->height*0.5f);
	GLfloat distanceToMiddleY = Planet.radius;

	for (i = 0; i < 4; ++i)
	{
		Planet.terrainModelToWorld[i] = T(-distanceToMiddleX, distanceToMiddleY, -distanceToMiddleZ);
		Planet.terrainModelToWorld[i] = Mult( Rz(M_PI*0.5f*(GLfloat)i), Planet.terrainModelToWorld[i] );
	}

	//Last two sides
	for (i = 0; i < 2; ++i)
	{
		Planet.terrainModelToWorld[4+i] = T(-distanceToMiddleX, distanceToMiddleY, -distanceToMiddleZ);
		Planet.terrainModelToWorld[4+i] = Mult( Rx(M_PI*(0.5f+(GLfloat)i)), Planet.terrainModelToWorld[4+i] );
	}

	//Weird offset: (Probably size dependent)
	Planet.terrainModelToWorld[1] = Mult(T(0, 1, 0), Planet.terrainModelToWorld[1]);
	Planet.terrainModelToWorld[2] = Mult(T(-1, 1, 0), Planet.terrainModelToWorld[2]);
	Planet.terrainModelToWorld[3] = Mult(T(-1, 0, 0), Planet.terrainModelToWorld[3]);
	Planet.terrainModelToWorld[4] = Mult(T(0, 0, -1), Planet.terrainModelToWorld[4]);
	Planet.terrainModelToWorld[5] = Mult(T(0, 1, 0), Planet.terrainModelToWorld[5]);



	//Generate terrain
	//TextureData* tex = chkmalloc(sizeof(TextureData));
	//GenerateProceduralTerrainTexture(256, tex);
	//Planet.terrainTexture[0] = *tex;
	
	for(i = 0; i < 6; i++)
	{
		Planet.terrainModels[i] = GenerateCubeTerrainSimple(Planet);

		Model* oldmodel = Planet.terrainModels[i]; //Try to prevent memory leaks (Need to find more)
		Planet.terrainModels[i] = MapCubeToSphere(Planet, i);
		free(oldmodel);

		Planet.terrainModelToWorld[i] = T(Planet.center.x, Planet.center.y, Planet.center.z);
	}

	planetsList[numberOfPlanets-1] = Planet;
}


void InitWindow(GLint width, GLint height)
{
	windowWidth = width;
	windowHeight = height;
	glViewport(0,0,windowWidth, windowHeight);
}


void SetCurrentPlanet(GLuint newCurrent)
{
	if(newCurrent < numberOfPlanets)
		currentPlanet = newCurrent;
}



void cleanUpAndExit()
{
	GLint i, j;
	for(j = 0; j < numberOfPlanets; j++)
	for(i = 0; i < 6; i++)
	{
		free(planetsList[j].terrainTexture[i]); //Should probably free all the member arrays too...
		free(planetsList[j].terrainModels[i]);
	}
	exit(0);
}