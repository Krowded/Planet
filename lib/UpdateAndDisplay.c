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
		total = Mult(camMatrix, Planet.terrainModelToWorld[i]);
		glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "mdlMatrix"), 1, GL_TRUE, total.m);
		DrawModel(terrainModel, terrainProgram, "inPosition", "inNormal", "inTexCoord");
	}

	//Draw sky
	glUseProgram(skyProgram);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	translation = T(0, 0, 0);
	rotation = Rx(0);
	model = Mult(translation, rotation);
	mv = Mult(camera, model);
	(mv.m)[3] = 0;
	(mv.m)[7] = 0;
	(mv.m)[11] = 0;
	mvp = Mult(projection, mv);
	glUniformMatrix4fv(glGetUniformLocation(skyProgram, "mvp"), 1, GL_TRUE, mvp.m);
	glBindTexture(GL_TEXTURE_2D, skytex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	DrawModel(skyboxm, skyProgram, "inPosition", NULL, "inTexCoord");
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);	
	

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
	UpdateCamera(t, Planet);
}



void timer(GLint integer)
{ 	
	globalTime = glutGet(GLUT_ELAPSED_TIME);
	Update(globalTime);

	glutPostRedisplay();

	glutTimerFunc(10, &timer, globalTime);
}