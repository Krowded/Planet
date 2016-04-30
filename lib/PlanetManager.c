#include "PlanetManager.h"
#include "Terrain.h"

#include "DisplayGlobals.h"
#include "ControlGlobals.h"

LOCAL void freeTexture(TextureData* texture);
LOCAL void freePlanet(struct PlanetStruct planet);

char* createPlanetNoise = "audio/1_person_cheering-Jett_Rifkin-1851518140.mp3";
char* deletePlanetNoise = "audio/Blop-Mark_DiAngelo-79054334.mp3";

GLuint numberOfPlanets = 0;
GLuint maxNumberOfPlanets = 8; //0 for infinite
GLuint currentPlanet = 0;


void CreateCubeHeightMaps(TextureData* terrainTextures[6], mat4 terrainTransformationMatrix[6], struct PlanetStruct* planet)
{
		GLuint i;	
		for(i = 0; i < 6; i++)
		{
			terrainTextures[i] = chkmalloc(sizeof(TextureData));
		}

		// Load terrain data
		for (i = 0; i < 6; i++)
			LoadTGATextureData("textures/fft-terrain.tga", terrainTextures[i]);

		//Generate terrain model matrix

		planet->terrainWidth = (GLfloat)terrainTextures[0]->width;
		planet->terrainHeight = (GLfloat)terrainTextures[0]->height;
		GLfloat distanceToMiddleX = ((GLfloat)terrainTextures[0]->width*0.5f);
		GLfloat distanceToMiddleZ = ((GLfloat)terrainTextures[0]->height*0.5f);
		GLfloat distanceToMiddleY = planet->radius;

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

}


/*
 *	Creates a new planet and appends it to planetsList
 */
void CreatePlanet(struct PlanetStruct planet, GLuint playSound)
{
	if(numberOfPlanets < maxNumberOfPlanets || maxNumberOfPlanets == 0)
	{
		numberOfPlanets++;
		planetsList = realloc(planetsList, sizeof(struct PlanetStruct)*numberOfPlanets);

		planet.startingPosition = planet.center;
		planet.timeOfCreation = glutGet(GLUT_ELAPSED_TIME);
		planet.upVec = SetVector(0,1,0);
		planet.frontVec = SetVector(0,0,1);

		mat4 terrainTransformationMatrix[6];
		TextureData* terrainTextures[6];

		CreateCubeHeightMaps(terrainTextures, terrainTransformationMatrix, &planet);


		//Generate terrain
		//TextureData* tex = chkmalloc(sizeof(TextureData));
		//GenerateProceduralTerrainTexture(256, tex);
		//terrainTextures[0] = *tex;
		GLuint i;
		for(i = 0; i < 6; i++)
		{
			planet.terrainModels[i] = GenerateCubeTerrainSimple(terrainTextures[i]);

			Model* oldmodel = planet.terrainModels[i]; //Try to prevent memory leaks (Need to find more)
			switch(planet.type)
			{
				case SMOOTH_PLANET:
					planet.terrainModels[i] = MapCubeToFlatSphere(planet, terrainTransformationMatrix, i);
					break;
				case ROUGH_PLANET:
					planet.terrainModels[i] = MapCubeToSphere(planet, terrainTransformationMatrix, i);
					break;
				default:
					fprintf(stderr, "Unknown planet type");
			}
			free(oldmodel);
			freeTexture(terrainTextures[i]); //Dont need them in memory anymore, free properly
		}

		planet.ModelToWorldMatrix = T(planet.center.x, planet.center.y, planet.center.z);

		planetsList[numberOfPlanets-1] = planet;

		//Decide if/what sound to play		
		if(playSound != NO_SOUND)
			PlayAudioFile(createPlanetNoise);

		printf("Let there be light!\n");
	}
	else
	{
		//Play fail noise?
		fprintf(stderr, "Max number of planets reached\n");
	}
}

/*
 *	Creates a sun and appends it to planetsList. A sun is a planet set at the origin that does not rotate or orbit.
 */
void CreateSun(GLfloat radius)
{
	struct PlanetStruct planet;
	planet.center = SetVector(0, 0, 0);
	planet.radius = radius;
	planet.orbitalSpeed = 0;
	planet.orbitalAxis = SetVector(0,1,0);
	planet.rotationalSpeed = 0;
	planet.rotationalAxis = SetVector(0,1,0);
	planet.type = SMOOTH_PLANET;

	CreatePlanet(planet, NO_SOUND);
}

void CreateStandardPlanet(vec3 center, GLfloat radius, GLuint playSound)
{
	struct PlanetStruct planet;
	planet.center = center;
	planet.radius = radius;
	planet.orbitalSpeed = 0.001;
	planet.orbitalAxis = SetVector(0,1,0);
	planet.rotationalSpeed = 0.001;
	planet.rotationalAxis = SetVector(1,1,0);
	planet.type = ROUGH_PLANET;

	CreatePlanet(planet, playSound);
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
		PlayAudioFile(deletePlanetNoise);
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

GLuint GetNumberOfPlanets()
{
	return numberOfPlanets;
}

GLuint GetCurrentPlanet()
{
	return currentPlanet;
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
LOCAL void freePlanet(struct PlanetStruct planet)
{
	GLuint i;
	for(i = 0; i < 6; i++)
	{
		free(planet.terrainModels[i]);
	}
}

void freeAllPlanets()
{
	GLuint i;
	for(i = 0; i < numberOfPlanets; i++)
		freePlanet(planetsList[i]);
}