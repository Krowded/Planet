#include "PlanetManager.h"
#include "Terrain.h"
#include "PlayAudioFile.h"

#include "DisplayGlobals.h"
#include "ControlGlobals.h"

LOCAL void deleteTexture(TextureData* texture);
LOCAL void deletePlanet(struct PlanetStruct planet);
LOCAL void deleteModel(Model* m);

char* createPlanetNoise = "audio/Bow_Fire_Arrow-Stephan_Schutze-2133929391.mp3";
char* deletePlanetNoise = "audio/Blop-Mark_DiAngelo-79054334.mp3";

GLuint numberOfPlanets = 0;
GLuint maxNumberOfPlanets = 8; //0 for infinite
GLuint currentPlanet = 0;
GLuint fuzzy = 0; //Use the failed randomly generated height map

void CreateCubeHeightMaps(TextureData* terrainTextures[6], mat4 terrainTransformationMatrix[6], struct PlanetStruct* planet)
{
		GLuint i;	
		for(i = 0; i < 6; i++)
		{
			terrainTextures[i] = chkmalloc(sizeof(TextureData));
		}

		// Load terrain data
		for (i = 0; i < 6; i++)
			if(fuzzy == 0)
				LoadTGATextureData("textures/fft-terrain.tga", terrainTextures[i]);
			else
				LoadTGATextureData("testTGA.tga", terrainTextures[i]);


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
		/*
		TextureData* tex = chkmalloc(sizeof(TextureData));
		GenerateProceduralTerrainTexture(256, tex);
		terrainTextures[0] = tex;*/
		GLuint i;
		for(i = 0; i < 6; i++)
		{
			planet.terrainModels[i] = GenerateTerrainModelSimple(terrainTextures[i], planet.textureScale);

			switch(planet.type)
			{
				case SMOOTH_PLANET:
					planet.terrainModels[i] = MapToFlatSphere(planet, terrainTransformationMatrix, terrainTextures[i], i);
					break;
				case ROUGH_PLANET:
					planet.terrainModels[i] = MapToSphere(planet, terrainTransformationMatrix, terrainTextures[i], i);
					break;
				case CUBE_PLANET:
					planet.terrainModels[i] = MapToCube(planet, terrainTransformationMatrix, terrainTextures[i], i);
					break;
				default:
					fprintf(stderr, "Unknown planet type");
			}
			deleteTexture(terrainTextures[i]); //Dont need them in memory anymore, free properly
		}

		planet.ModelToWorldMatrix = T(planet.center.x, planet.center.y, planet.center.z);

		planetsList[numberOfPlanets-1] = planet;

		//Decide if/what sound to play		
		if(playSound != NO_SOUND)
			PlayAudioFile(createPlanetNoise);

		//printf("Let there be light!\n");
	}
	else
	{
		//Play fail noise?
		//fprintf(stderr, "Max number of planets reached\n");
	}
}

/*
 *	Creates a sun and appends it to planetsList.
 */
void CreateSun(GLfloat radius, enum planetShape shape)
{
	struct PlanetStruct planet;
	planet.center = SetVector(0, 0, 0);
	planet.startingPosition = planet.center;
	planet.radius = radius;
	planet.orbitalSpeed = 0;
	planet.orbitalAxis = SetVector(0,1,0);
	planet.rotationalSpeed = 0.0002;
	planet.rotationalAxis = SetVector(1,1,0);
	planet.type = shape;
	planet.textureScale = 1;

	CreatePlanet(planet, NO_SOUND);
}

void CreateStandardPlanet(vec3 center, GLfloat radius, enum planetShape shape, GLuint playSound)
{
	struct PlanetStruct planet;
	planet.center = center;
	planet.radius = radius;
	planet.orbitalSpeed = 0.001;
	planet.orbitalAxis = SetVector(0,1,0);
	planet.rotationalSpeed = 0.001;
	planet.rotationalAxis = SetVector(1,1,0);
	planet.type = shape;
	planet.textureScale = 1;

	if(shape == CUBE_PLANET) //Doesn't scale properly atm
		planet.radius = 128;

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
		deletePlanet(planetsList[numberOfPlanets-1]);
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

		if(!(planetsList[i].startingPosition.x == planetsList[0].startingPosition.x 
		  && planetsList[i].startingPosition.y == planetsList[0].startingPosition.y 
		  && planetsList[i].startingPosition.z == planetsList[0].startingPosition.z )) //Dont try to orbit when already at 0
		{	
			if(fabs(planetsList[i].startingPosition.x) - arbAxis.x + 
			   fabs(planetsList[i].startingPosition.y) - arbAxis.y + 
			   fabs(planetsList[i].startingPosition.z) - arbAxis.z == 0)
				arbAxis = SetVector(1,0,0);

			arbAxis = Normalize(CrossProduct(VectorSub(planetsList[i].startingPosition, planetsList[0].startingPosition), arbAxis));

			ModelToWorld = Mult(T(planetsList[0].center.x, planetsList[0].center.y, planetsList[0].center.z), ModelToWorld);
			ModelToWorld = Mult(ArbRotate(arbAxis, planetsList[i].orbitalSpeed*(GLfloat)t), ModelToWorld); //Orbit
			ModelToWorld = Mult(T(-planetsList[0].center.x, -planetsList[0].center.y, -planetsList[0].center.z), ModelToWorld);
		}
		planetsList[i].ModelToWorldMatrix = ModelToWorld;
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
LOCAL void deleteTexture(TextureData* texture)
{
	free(texture->imageData);
	free(texture);
}

//Fixed version of DisposeModel
LOCAL void deleteModel(Model* m)
{
		if (m->vertexArray != NULL)
			free(m->vertexArray);
		if (m->normalArray != NULL)
			free(m->normalArray);
		if (m->texCoordArray != NULL)
			free(m->texCoordArray);
		if (m->colorArray != NULL) // obsolete?
			free(m->colorArray);
		if (m->indexArray != NULL)
			free(m->indexArray);
			
		// Lazy error checking heter since "glDeleteBuffers silently ignores 0's and names that do not correspond to existing buffer objects."
		
		glDeleteBuffers(1, &m->vb);
		glDeleteBuffers(1, &m->ib);
		glDeleteBuffers(1, &m->nb);
		glDeleteBuffers(1, &m->tb);
		glDeleteVertexArrays(1, &m->vao);

		free(m);
}

/*
 *	Safely frees a PlanetStruct
 */
LOCAL void deletePlanet(struct PlanetStruct planet)
{
	GLuint i;
	for(i = 0; i < 6; i++)
	{
		deleteModel(planet.terrainModels[i]);
	}
}

void deleteAllPlanets()
{
	GLuint i;
	for(i = 0; i < numberOfPlanets; i++)
		deletePlanet(planetsList[i]);
}