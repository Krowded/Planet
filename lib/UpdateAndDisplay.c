#include "UpdateAndDisplay.h"

void PlanetMovement(GLint t)
{
	GLint i, j;
	mat4 ModelToWorld;
	for(i = 0; i < numberOfPlanets; i++)
		for(j = 0; j < 6; j++)
		{	
			t = glutGet(GLUT_ELAPSED_TIME) -  planetsList[i].timeOfCreation;

			vec3 arbAxis = {0,1,0};
			ModelToWorld = ArbRotate(planetsList[i].rotationalAxis, planetsList[i].rotationalSpeed*(GLfloat)t); //IdentityMatrix(); //Rotation around own axis
			ModelToWorld = Mult(T(planetsList[i].center.x, planetsList[i].center.y, planetsList[i].center.z), ModelToWorld); //Offset

			if(!(planetsList[i].center.x == 0 && planetsList[i].center.y == 0 && planetsList[i].center.z == 0)) //Dont try to orbit when already at 0
			{	
				if(abs(planetsList[i].center.x) - arbAxis.x + abs(planetsList[i].center.y) - arbAxis.y + abs(planetsList[i].center.z) - arbAxis.z == 0)
					arbAxis = SetVector(1,0,0);

				arbAxis = Normalize(CrossProduct(planetsList[i].center, arbAxis));

				ModelToWorld = Mult(ArbRotate(arbAxis, planetsList[i].orbitalSpeed*(GLfloat)t), ModelToWorld); //Orbit
				planetsList[i].terrainModelToWorld[j] = ModelToWorld;
			}
		}
}


void display(void)
{
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	mat4 total, ModelToWorld;
	
	printError("pre display");
	

	//Draw terrain
	glUseProgram(terrainProgram);

	
	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	PlanetMovement(globalTime);
	GLint i,j;
	for(j = 0; j < numberOfPlanets; j++)
		for(i = 0; i < 6; ++i)
		{
			total = Mult(camMatrix, planetsList[j].terrainModelToWorld[i]);
			//total = camMatrix;
			glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "mdlMatrix"), 1, GL_TRUE, total.m);
			DrawModel(planetsList[j].terrainModels[i], terrainProgram, "inPosition", "inNormal", "inTexCoord");
		}

/*
	//Draw models
	glUseProgram(modelProgram);

	//Sphere
	ModelToWorld = IdentityMatrix();
	ModelToWorld = Mult( T(25*globalTime*0.0001, 0, 0), ModelToWorld);
	vec3 currentPosition = GetCurrentPosition(ModelToWorld);
	//ModelToWorld = AdjustModelToHeightMap(ModelToWorld, 
	//	 								  GetTerrainHeight(currentPosition, terrainModel, Planet.terrainTexture[0]));
	total = Mult(camMatrix, ModelToWorld);
	glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mdlMatrix"), 1, GL_TRUE, total.m);
	DrawModel(m, modelProgram, "inPosition", "inNormal", "inTexCoord");

	//Octagon
	ModelToWorld = T( 3, 0, 0);
	total = Mult(camMatrix, ModelToWorld);
	glUniformMatrix4fv(glGetUniformLocation(modelProgram, "mdlMatrix"), 1, GL_TRUE, total.m);
	DrawModel(m2, modelProgram, "inPosition", "inNormal", "inTexCoord");
	*/



	printError("display 2");
	
	glutSwapBuffers();
	
}




//Update position and rotation of camera
void MouseUpdate(GLint mouseX, GLint mouseY)
{
	CameraMouseUpdate(mouseX, mouseY);
}

LOCAL void Update(GLint t)
{
	//Read input and update
	GLint x,y;
	getCursorPosition(&x, &y);
	MouseUpdate(x,y);
	UpdateCamera(t, planetsList[currentPlanet]);
}



void timer(GLint integer)
{ 	
	globalTime = glutGet(GLUT_ELAPSED_TIME);
	Update(globalTime);

	glutPostRedisplay();

	glutTimerFunc(10, &timer, globalTime);
}