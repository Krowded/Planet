#include "UpdateAndDisplay.h"
#include "VectorUtils3.h"
#include "CameraControl.h"
#include "DisplayGlobals.h"
#include "Init.h"
#include "MicroGlut.h"

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

	UpdatePlanetMovement(globalTime);
	GLint i,j;
	for(j = 0; j < numberOfPlanets; j++)
	{
		planetsList[j].center = vec4tovec3(MultVec4(planetsList[j].ModelToWorldMatrix, vec3tovec4(planetsList[j].center)));
		total = Mult(camMatrix, planetsList[j].ModelToWorldMatrix);
		glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "mdlMatrix"), 1, GL_TRUE, total.m);
		for(i = 0; i < 6; ++i)
		{
			DrawModel(planetsList[j].terrainModels[i], terrainProgram, "inPosition", "inNormal", "inTexCoord");
		}
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