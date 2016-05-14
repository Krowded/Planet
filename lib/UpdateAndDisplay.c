#include "UpdateAndDisplay.h"
#include "VectorUtils3.h"
#include "CameraControl.h"
#include "DisplayGlobals.h"
#include "Init.h"
#include "MicroGlut.h"
#include "Terrain.h"
#include "Skybox.h"



LOCAL void MouseUpdate(GLint mouseX, GLint mouseY);
LOCAL void Update(GLint t);


void display(void)
{
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Update projection
	projectionMatrix = perspective(fov, windowWidth/windowHeight, nearDrawDistance, drawDistance);


	printError("pre display");

	//Skybox
	glDisable(GL_DEPTH_TEST);
	glUseProgram(skyboxProgram);
	glBindTexture(GL_TEXTURE_2D, sunTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glUniform1i(glGetUniformLocation(skyboxProgram, "tex"), 0); // Texture unit 0	

	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "ViewToProjection"), 1, GL_TRUE, projectionMatrix.m);	
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "WorldToView"), 1, GL_TRUE, camMatrix.m);

	mat4 m = IdentityMatrix();
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "ModelToWorld"), 1, GL_TRUE, m.m);
	//glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "ModelToWorld"), 1, GL_TRUE, planetsList[0].ModelToWorldMatrix.m);
	
	glBindTexture(GL_TEXTURE_2D, skyboxTopTexture);
	DrawModel(skyTop, skyboxProgram, "inPosition", NULL, "inTexCoord");

	glBindTexture(GL_TEXTURE_2D, skyboxBottomTexture);
	DrawModel(skyBottom, skyboxProgram, "inPosition", NULL, "inTexCoord");
 	glBindTexture(GL_TEXTURE_2D, skyboxLeftTexture);
	DrawModel(skyLeft, skyboxProgram, "inPosition", NULL, "inTexCoord");
	glBindTexture(GL_TEXTURE_2D, skyboxRightTexture);
	DrawModel(skyRight, skyboxProgram, "inPosition", NULL, "inTexCoord");
	glBindTexture(GL_TEXTURE_2D, skyboxFrontTexture);
	DrawModel(skyFront, skyboxProgram, "inPosition", NULL, "inTexCoord");
	glBindTexture(GL_TEXTURE_2D, skyboxBackTexture);
	DrawModel(skyBack, skyboxProgram, "inPosition", NULL, "inTexCoord");
	
	glEnable(GL_DEPTH_TEST);


	

	

	//Draw sun
	if(GetNumberOfPlanets() > 0)
	{	
		glUseProgram(sunProgram);

		glBindTexture(GL_TEXTURE_2D, sunTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glUniform1i(glGetUniformLocation(sunProgram, "tex"), 0); // Texture unit 0	

		glUniformMatrix4fv(glGetUniformLocation(sunProgram, "ViewToProjection"), 1, GL_TRUE, projectionMatrix.m);	
		glUniformMatrix4fv(glGetUniformLocation(sunProgram, "WorldToView"), 1, GL_TRUE, camMatrix.m);


		planetsList[0].center = vec4tovec3(MultVec4(planetsList[0].ModelToWorldMatrix, vec3tovec4(planetsList[0].startingPosition)));

		glUniformMatrix4fv(glGetUniformLocation(sunProgram, "ModelToWorld"), 1, GL_TRUE, planetsList[0].ModelToWorldMatrix.m);
		GLuint i;
		for(i = 0; i < 6; ++i)
		{
			DrawModel(planetsList[0].terrainModels[i], sunProgram, "inPosition", NULL, "inTexCoord");
		}	

		//Draw planets
		glUseProgram(terrainProgram);

		glBindTexture(GL_TEXTURE_2D, tex1);	// Bind Our Texture tex1
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glUniform1i(glGetUniformLocation(terrainProgram, "tex"), 0); // Texture unit 0	

		glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "ViewToProjection"), 1, GL_TRUE, projectionMatrix.m);
		glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "WorldToView"), 1, GL_TRUE, camMatrix.m);

		GLuint j;
		for(j = 1; j < GetNumberOfPlanets(); j++)
		{
			planetsList[j].center = vec4tovec3(MultVec4(planetsList[j].ModelToWorldMatrix, vec3tovec4(planetsList[j].startingPosition)));

			vec3 sunlight = Normalize(VectorSub(planetsList[0].center, planetsList[j].center));
			glUniform3f(glGetUniformLocation(terrainProgram, "SunlightDirection"), sunlight.x, sunlight.y, sunlight.z);
			glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "ModelToWorld"), 1, GL_TRUE, planetsList[j].ModelToWorldMatrix.m);
			for(i = 0; i < 6; ++i)
			{
				DrawModel(planetsList[j].terrainModels[i], terrainProgram, "inPosition", "inNormal", "inTexCoord");
			}
		}
	}
	
	printError("display 2");
	
	glutSwapBuffers();
	
}


LOCAL void MouseUpdate(GLint mouseX, GLint mouseY)
{
	CameraMouseUpdate(mouseX, mouseY);
}


LOCAL void Update(GLint t)
{
	//Read input and update
	GLint x,y;
	getCursorPosition(&x, &y);
	MouseUpdate(x,y);
	UpdateCamera(t, planetsList[GetCurrentPlanet()]);
	UpdatePlanetMovement(t);
}


void timer(GLint time)
{ 	
	GLint t = glutGet(GLUT_ELAPSED_TIME);
	Update(t);

	glutPostRedisplay();

	glutTimerFunc(10, timer, t);
}