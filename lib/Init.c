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

struct planetStruct Planet;

struct planetStruct* planetsList;
GLint numberOfPlanets = 5;
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
	planetsList = malloc(sizeof(struct planetStruct)*numberOfPlanets);


	GLint j;
	for(j = 0; j<numberOfPlanets; j++){

	Planet = *(struct planetStruct*)chkmalloc(sizeof(struct planetStruct));


	Planet.center = SetVector(j*500,0,0);
	Planet.radius = 256.0f*0.5f;
	Planet.upVec = SetVector(0, 1, 0);
	Planet.frontVec = SetVector(0, 0, 1);
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

	planetsList[j] = Planet;
	}
}

/*
LOCAL void InitTerrain()
{
	//Allocate
	planetsList = malloc(sizeof(struct planetStruct)*numberOfPlanets);


	//Fill list
	GLint j,i;
	for(j = 0; j < numberOfPlanets; j++)
	{	
		struct planetStruct newPlanet;
		newPlanet.center = SetVector(0 + j*300,0,0);


		newPlanet.radius = 256.0f*0.5f;
		newPlanet.upVec = SetVector(0, 1, 0);
		newPlanet.frontVec = SetVector(0, 0, 1);
		for(i = 0; i < 6; i++)
		{
			newPlanet.terrainTexture[i] = chkmalloc(sizeof(TextureData));
		}

		// Load terrain data
		for (i = 0; i < 6; i++)
			LoadTGATextureData("textures/fft-terrain.tga", newPlanet.terrainTexture[i]);

		//Generate terrain model matrix
		GLfloat distanceToMiddleX = ((GLfloat)newPlanet.terrainTexture[0]->width*0.5f);
		GLfloat distanceToMiddleZ = ((GLfloat)newPlanet.terrainTexture[0]->height*0.5f);
		GLfloat distanceToMiddleY = newPlanet.radius;

		for (i = 0; i < 4; ++i)
		{
			newPlanet.terrainModelToWorld[i] = T(-distanceToMiddleX, distanceToMiddleY, -distanceToMiddleZ);
			newPlanet.terrainModelToWorld[i] = Mult( Rz(M_PI*0.5f*(GLfloat)i), newPlanet.terrainModelToWorld[i] );
		}

		//Last two sides
		for (i = 0; i < 2; ++i)
		{
			newPlanet.terrainModelToWorld[4+i] = T(-distanceToMiddleX, distanceToMiddleY, -distanceToMiddleZ);
			newPlanet.terrainModelToWorld[4+i] = Mult( Rx(M_PI*(0.5f+(GLfloat)i)), newPlanet.terrainModelToWorld[4+i] );
		}

		//Weird offset: (Probably size dependent)
		newPlanet.terrainModelToWorld[1] = Mult(T(0, 1, 0), newPlanet.terrainModelToWorld[1]);
		newPlanet.terrainModelToWorld[2] = Mult(T(-1, 1, 0), newPlanet.terrainModelToWorld[2]);
		newPlanet.terrainModelToWorld[3] = Mult(T(-1, 0, 0), newPlanet.terrainModelToWorld[3]);
		newPlanet.terrainModelToWorld[4] = Mult(T(0, 0, -1), newPlanet.terrainModelToWorld[4]);
		newPlanet.terrainModelToWorld[5] = Mult(T(0, 1, 0), newPlanet.terrainModelToWorld[5]);



		//Generate terrain
		//TextureData* tex = chkmalloc(sizeof(TextureData));
		//GenerateProceduralTerrainTexture(256, tex);
		//Planet.terrainTexture[0] = *tex;
		
		for(i = 0; i < 6; i++)
		{
			newPlanet.terrainModels[i] = GenerateCubeTerrainSimple(newPlanet);

			Model* oldmodel = newPlanet.terrainModels[i]; //Try to prevent memory leaks (Need to find more)
			Planet.terrainModels[i] = MapCubeToSphere(newPlanet, i);
			free(oldmodel);

			newPlanet.terrainModelToWorld[i] = T(newPlanet.center.x, newPlanet.center.y, newPlanet.center.z);
		}
		fprintf(stderr, "help\n");
		planetsList[j] = newPlanet;
	}
}
*/

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
	int i;
	for(i = 0; i < 6; i++)
	{
		free(Planet.terrainTexture[i]); //Should probably free all the member arrays too...
		free(Planet.terrainModels[i]);
	}
	exit(0);
}