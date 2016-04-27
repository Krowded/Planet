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

GLfloat terrainScale = 10.0;
GLint roundingDistanceFromEdge = 1;
GLfloat simpleInterpolationHeight = 10;

GLint globalTime = 0;
mat4 camMatrix, camBaseMatrix, projectionMatrix;	
Model *m, *m2, *terrainModel;
GLuint terrainProgram, modelProgram;
GLuint tex1, tex2;


GLfloat maxAngleOfTerrain = 0.78539816339;

struct planetStruct Planet;

void InitAll()
{
	InitWindow(windowWidth, windowHeight);
	Planet.center = SetVector(0,0,0);
	Planet.radius = 256.0f*0.5f;
	Planet.upVec = SetVector(0, 1, 0);
	Planet.frontVec = SetVector(0, 0, 1);
	GLint i;
	for(i = 0; i < 6; i++)
	{
		Planet.terrainTexture[i] = chkmalloc(sizeof(TextureData));
	}


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
	// Load terrain data
	GLint i;
	for (i = 0; i < 6; i++)
		LoadTGATextureData("textures/fft-terrain.tga", Planet.terrainTexture[i]);

	//Generate terrain model matrix
	GLfloat distanceToMiddleX = ((GLfloat)Planet.terrainTexture[0]->width*0.5f);
	GLfloat distanceToMiddleZ = ((GLfloat)Planet.terrainTexture[0]->height*0.5f);
	GLfloat distanceToMiddleY = Planet.radius;//127.5;

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
		Planet.terrainModels[i] = GenerateCubeTerrainSimple(&Planet);

		Model* oldmodel = Planet.terrainModels[i]; //Try to prevent memory leaks (Need to find more)
		Planet.terrainModels[i] = MapCubeToSphere(Planet, i);
		free(oldmodel);
	}

	

}


void InitWindow(GLint width, GLint height)
{
	windowWidth = width;
	windowHeight = height;
	glViewport(0,0,windowWidth, windowHeight);
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