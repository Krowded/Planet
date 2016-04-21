#include "UpdateAndDisplay.h"

void display(void)
{
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	mat4 total, ModelToWorld;
	
	printError("pre display");
	

	//Draw terrain
	glUseProgram(terrainProgram);

	
	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
	GLint i;
	for (i = 0; i < 6; ++i)
	{
		total = Mult(camMatrix, Planet.TerrainModelToWorld[i]);
		glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "mdlMatrix"), 1, GL_TRUE, total.m);
		DrawModel(terrainModel, terrainProgram, "inPosition", "inNormal", "inTexCoord");
	}

	//Match gravity to planet (should it be here?)			
	vec3 offvec = GetCurrentPosition(Planet.TerrainModelToWorld[0]); //Has to be the model matrix for the one you're walking on
	SetTerrainOffset(offvec.x, offvec.y, offvec.z);


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
	UpdateCamera(t);
}



void timer(GLint integer)
{ 	
	globalTime = glutGet(GLUT_ELAPSED_TIME);
	Update(globalTime);

	glutPostRedisplay();

	glutTimerFunc(10, &timer, globalTime);
}