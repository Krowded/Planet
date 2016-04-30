#include "PlanetManager.h"
#include "Terrain.h"

#include "DisplayGlobals.h"
#include "ControlGlobals.h"

LOCAL void freeTexture(TextureData* texture);



/*
 *	Creates a new planet and appends it to planetsList
 */
void CreatePlanet(vec3 center, GLfloat radius, vec3 upVec, vec3 frontVec, GLfloat orbitalSpeed, vec3 orbitalAxis, GLfloat rotationalSpeed, vec3 rotationalAxis)
{
	numberOfPlanets++;
	planetsList = realloc(planetsList, sizeof(struct PlanetStruct)*numberOfPlanets);

	struct PlanetStruct Planet;
	Planet.center = center;
	Planet.startingPosition = center;
	Planet.radius = radius;
	Planet.upVec = upVec;
	Planet.frontVec = frontVec;
	Planet.orbitalSpeed = orbitalSpeed;
	Planet.orbitalAxis = orbitalAxis;
	Planet.rotationalSpeed = rotationalSpeed;
	Planet.rotationalAxis = rotationalAxis;
	Planet.timeOfCreation = glutGet(GLUT_ELAPSED_TIME);
	

	mat4 terrainTransformationMatrix[6];
	TextureData* terrainTextures[6];

	GLuint i;	
	for(i = 0; i < 6; i++)
	{
		terrainTextures[i] = chkmalloc(sizeof(TextureData));
	}

	// Load terrain data
	for (i = 0; i < 6; i++)
		LoadTGATextureData("textures/fft-terrain.tga", terrainTextures[i]);

	//Generate terrain model matrix

	Planet.terrainWidth = (GLfloat)terrainTextures[0]->width;
	Planet.terrainHeight = (GLfloat)terrainTextures[0]->height;
	GLfloat distanceToMiddleX = ((GLfloat)terrainTextures[0]->width*0.5f);
	GLfloat distanceToMiddleZ = ((GLfloat)terrainTextures[0]->height*0.5f);
	GLfloat distanceToMiddleY = Planet.radius;

	for (i = 0; i < 4; ++i)
	{
		terrainTransformationMatrix[i] = T(-distanceToMiddleX, distanceToMiddleY, -distanceToMiddleZ);
		terrainTransformationMatrix[i] = Mult( Rz(M_PI*0.5f*(GLfloat)i), terrainTransformationMatrix[i] );
	}

	//Last two sides
	for (i = 0; i < 2; ++i)
	{
		terrainTransformationMatrix[4+i] = T(-distanceToMiddleX, distanceToMiddleY, -distanceToMiddleZ);
		terrainTransformationMatrix[4+i] = Mult( Rx(M_PI*(0.5f+(GLfloat)i)), terrainTransformationMatrix[4+i] );
	}

	//Weird offset: (Probably size dependent)
	terrainTransformationMatrix[1] = Mult(T(0, 1, 0), terrainTransformationMatrix[1]);
	terrainTransformationMatrix[2] = Mult(T(-1, 1, 0), terrainTransformationMatrix[2]);
	terrainTransformationMatrix[3] = Mult(T(-1, 0, 0), terrainTransformationMatrix[3]);
	terrainTransformationMatrix[4] = Mult(T(0, 0, -1), terrainTransformationMatrix[4]);
	terrainTransformationMatrix[5] = Mult(T(0, 1, 0), terrainTransformationMatrix[5]);



	//Generate terrain
	//TextureData* tex = chkmalloc(sizeof(TextureData));
	//GenerateProceduralTerrainTexture(256, tex);
	//terrainTextures[0] = *tex;
	
	for(i = 0; i < 6; i++)
	{
		Planet.terrainModels[i] = GenerateCubeTerrainSimple(terrainTextures[i]);

		Model* oldmodel = Planet.terrainModels[i]; //Try to prevent memory leaks (Need to find more)
		Planet.terrainModels[i] = MapCubeToSphere(Planet, terrainTransformationMatrix, i);
		free(oldmodel);
		freeTexture(terrainTextures[i]); //Dont need them in memory anymore, free properly
	}

	Planet.ModelToWorldMatrix = T(Planet.center.x, Planet.center.y, Planet.center.z);

	planetsList[numberOfPlanets-1] = Planet;
}

/*
 *	Creates a sun and appends it to planetsList. A sun is a planet set at the origin that does not rotate or orbit.
 */
void CreateSun(GLfloat radius)
{
	numberOfPlanets++;
	planetsList = realloc(planetsList, sizeof(struct PlanetStruct)*numberOfPlanets);

	struct PlanetStruct Planet;
	Planet.center = SetVector(0,0,0);
	Planet.startingPosition = Planet.center;
	Planet.radius = radius;
	Planet.upVec = SetVector(0,1,0);
	Planet.frontVec = SetVector(0,0,1);
	Planet.orbitalSpeed = 0;
	Planet.orbitalAxis = SetVector(0,1,0);
	Planet.rotationalSpeed = 0;
	Planet.rotationalAxis = SetVector(0,1,0);
	Planet.timeOfCreation = glutGet(GLUT_ELAPSED_TIME);

	mat4 terrainTransformationMatrix[6];
	TextureData* terrainTextures[6];


	GLuint i;	
	for(i = 0; i < 6; i++)
	{
		terrainTextures[i] = chkmalloc(sizeof(TextureData));
	}

	// Load terrain data
	for (i = 0; i < 6; i++)
		LoadTGATextureData("textures/fft-terrain.tga", terrainTextures[i]);

	//Generate terrain model matrix

	Planet.terrainWidth = (GLfloat)terrainTextures[0]->width;
	Planet.terrainHeight = (GLfloat)terrainTextures[0]->height;

	GLfloat distanceToMiddleX = ((GLfloat)terrainTextures[0]->width*0.5f);
	GLfloat distanceToMiddleZ = ((GLfloat)terrainTextures[0]->height*0.5f);
	GLfloat distanceToMiddleY = Planet.radius;

	for (i = 0; i < 4; ++i)
	{
		terrainTransformationMatrix[i] = T(-distanceToMiddleX, distanceToMiddleY, -distanceToMiddleZ);
		terrainTransformationMatrix[i] = Mult( Rz(M_PI*0.5f*(GLfloat)i), terrainTransformationMatrix[i] );
	}

	//Last two sides
	for (i = 0; i < 2; ++i)
	{
		terrainTransformationMatrix[4+i] = T(-distanceToMiddleX, distanceToMiddleY, -distanceToMiddleZ);
		terrainTransformationMatrix[4+i] = Mult( Rx(M_PI*(0.5f+(GLfloat)i)), terrainTransformationMatrix[4+i] );
	}

	//Weird offset: (Probably size dependent)
	terrainTransformationMatrix[1] = Mult(T(0, 1, 0), terrainTransformationMatrix[1]);
	terrainTransformationMatrix[2] = Mult(T(-1, 1, 0), terrainTransformationMatrix[2]);
	terrainTransformationMatrix[3] = Mult(T(-1, 0, 0), terrainTransformationMatrix[3]);
	terrainTransformationMatrix[4] = Mult(T(0, 0, -1), terrainTransformationMatrix[4]);
	terrainTransformationMatrix[5] = Mult(T(0, 1, 0), terrainTransformationMatrix[5]);



	//Generate terrain
	//TextureData* tex = chkmalloc(sizeof(TextureData));
	//GenerateProceduralTerrainTexture(256, tex);
	//terrainTextures[0] = *tex;
	
	for(i = 0; i < 6; i++)
	{
		Planet.terrainModels[i] = GenerateCubeTerrainSimple(terrainTextures[i]);

		Model* oldmodel = Planet.terrainModels[i]; //Try to prevent memory leaks (Need to find more)
		Planet.terrainModels[i] = MapCubeToFlatSphere(Planet, terrainTransformationMatrix, i);
		free(oldmodel);
		freeTexture(terrainTextures[i]); //Dont need them in memory anymore, free properly
	}

	Planet.ModelToWorldMatrix = T(Planet.center.x, Planet.center.y, Planet.center.z);



	planetsList[numberOfPlanets-1] = Planet;
}


/*
 *	Changes the active planet, i.e. which planet gravity applies to
 */
void SetCurrentPlanet(GLuint newCurrent)
{
	fprintf(stderr, "Planet switch!\n");
	if(newCurrent < numberOfPlanets && newCurrent != currentPlanet)
	{
		currentPlanet = newCurrent;
		NewGravity = GL_TRUE;
	}
}

/*
 *	Removes the last planet from planetsList
 */
void RemoveLastPlanet()
{
	if(numberOfPlanets > 0)
	{
		freePlanet(planetsList[numberOfPlanets-1]);
		numberOfPlanets--;
		if(numberOfPlanets != 0) //Because segfault if realloced to 0
			planetsList = realloc(planetsList, sizeof(struct PlanetStruct)*numberOfPlanets);
	}
}

/*
 *	Takes care of orbit and rotation
 */
void UpdatePlanetMovement(GLint t)
{
	GLuint i;
	mat4 ModelToWorld;
	for(i = 0; i < numberOfPlanets; i++)
	{
		t = glutGet(GLUT_ELAPSED_TIME) -  planetsList[i].timeOfCreation;

		vec3 arbAxis = {0,1,0};
		ModelToWorld = ArbRotate(planetsList[i].rotationalAxis, planetsList[i].rotationalSpeed*(GLfloat)t); //IdentityMatrix(); //Rotation around own axis
		ModelToWorld = Mult(T(planetsList[i].startingPosition.x, planetsList[i].startingPosition.y, planetsList[i].startingPosition.z), ModelToWorld); //Offset

		if(!(planetsList[i].startingPosition.x == 0 && planetsList[i].startingPosition.y == 0 && planetsList[i].startingPosition.z == 0)) //Dont try to orbit when already at 0
		{	
			if(fabs(planetsList[i].startingPosition.x) - arbAxis.x + fabs(planetsList[i].startingPosition.y) - arbAxis.y + fabs(planetsList[i].startingPosition.z) - arbAxis.z == 0)
				arbAxis = SetVector(1,0,0);

			arbAxis = Normalize(CrossProduct(planetsList[i].startingPosition, arbAxis));

			ModelToWorld = Mult(ArbRotate(arbAxis, planetsList[i].orbitalSpeed*(GLfloat)t), ModelToWorld); //Orbit
			planetsList[i].ModelToWorldMatrix = ModelToWorld;
		}
	}
}


/*
 *	Safely frees a TextureData structure
 */
LOCAL void freeTexture(TextureData* texture)
{
	free(texture->imageData);
	free(texture);
}

/*
 *	Safely frees a PlanetStruct
 */
void freePlanet(struct PlanetStruct planet)
{
	GLuint i;
	for(i = 0; i < 6; i++)
	{
		free(planet.terrainModels[i]);
	}
}