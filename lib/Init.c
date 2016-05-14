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
Model *m, *m2, *skyboxm,*terrainModel;
GLuint terrainProgram, modelProgram, skyProgram;
GLuint tex1, tex2, skytex1, skytex2, skytex3, skytex4, skytex5, skytex6;
TextureData terrainTexture;

const GLfloat radius = 128;
GLfloat maxAngleOfTerrain = 0.78539816339;


struct planetStruct Planet;

void InitAll()
{
	InitWindow(windowWidth, windowHeight);
	Planet.center = SetVector(0,0,0);
	Planet.radius = radius;
	Planet.upVec = SetVector(0, 1, 0);
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
	skyProgram = loadShaders("shaders/sky.vert", "shaders/sky.frag");
	glUseProgram(terrainProgram);
	printError("Init shader");
}

LOCAL void InitTextures()
{
	glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(terrainProgram, "tex"), 0); // Texture unit 0
	LoadTGATextureSimple("textures/maskros512.tga", &tex1);
	LoadTGATextureSimple("textures/skyboxtest_right1.tga", &skytex1);
	LoadTGATextureSimple("textures/skyboxtest_left2.tga", &skytex2);
	LoadTGATextureSimple("textures/skyboxtest_top3.tga", &skytex3);
	LoadTGATextureSimple("textures/skyboxtest_bottom4.tga", &skytex4);
	LoadTGATextureSimple("textures/skyboxtest_front5.tga", &skytex5);
	LoadTGATextureSimple("textures/skyboxtest_back6.tga", &skytex6);

	
	

	printError("Init terrain");	
}

void InitModels()
{
	// Load models
	m = LoadModelPlus("models/groundsphere.obj");
	m2 = LoadModelPlus("models/octagon.obj");
	skyboxm = LoadModelPlus("models/skybox.obj");
}


LOCAL void InitTerrain()
{
	// Load terrain data
	GLint i;
	for (i = 0; i < 6; i++)
		LoadTGATextureData("textures/fft-terrain.tga", &(Planet.terrainTexture[i]));

	//Generate terrain model matrix
	GLfloat overlap = 0;
	GLfloat distanceToMiddleX = ((GLfloat)Planet.terrainTexture[0].width*0.5);
	GLfloat distanceToMiddleZ = ((GLfloat)Planet.terrainTexture[0].height*0.5);
	GLfloat distanceToMiddleY = distanceToMiddleX - overlap;


	for (i = 0; i < 4; ++i)
	{
		Planet.terrainModelToWorld[i] = T(-distanceToMiddleX, distanceToMiddleY, -distanceToMiddleZ);
		Planet.terrainModelToWorld[i] = Mult( Rz(M_PI*0.5f*(GLfloat)i), Planet.terrainModelToWorld[i] );
	}
	//Last two sides

	for (i = 0; i < 2; ++i)
	{
		Planet.terrainModelToWorld[4+i] = T(-distanceToMiddleX, distanceToMiddleY, -distanceToMiddleZ);
		//Planet.terrainModelToWorld[4+i] = Mult( Rx(M_PI*0.5f*(GLfloat)(i+1)), Planet.terrainModelToWorld[3+i] );
	}

		Planet.terrainModelToWorld[4] = Mult( Rx(M_PI*0.5f*(GLfloat)(1)), Planet.terrainModelToWorld[4] );
		Planet.terrainModelToWorld[5] = Mult( Rx(M_PI*0.5f*(GLfloat)(3)), Planet.terrainModelToWorld[5] );


	//Generate terrain
	//TextureData* tex = malloc(sizeof(TextureData));
	//GenerateProceduralTerrainTexture(256, tex);
	//Planet.terrainTexture[0] = *tex;
	
	terrainModel = GenerateCubeTerrainSimple(&Planet);
	//terrainModel = GenerateTerrainFromTexture(tex);
	//terrainModel = GenerateCubeTerrain(&Planet);
	terrainModel = MapCubeToFlatSphere(terrainModel, Planet.radius, Planet.terrainTexture[0].width, Planet.terrainTexture[0].height);
}


void InitWindow(GLint width, GLint height)
{
	windowWidth = width;
	windowHeight = height;
	glViewport(0,0,windowWidth, windowHeight);
}