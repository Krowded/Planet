#include "UpdateAndDisplay.h"
#include "BoxForSky.c"

void display(void)
{
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	mat4 total, ModelToWorld, skyTranslation, skyRotation, skyModel, mv, mvp;
	
	printError("pre display");
	
//Draw sky
	glUseProgram(skyProgram);
	glDisable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);
	skyTranslation = T(0, 0, 0);
	skyRotation = Rx(0);
	skyModel = Mult(skyTranslation, skyRotation);
	mv = Mult(camMatrix, skyModel);
	(mv.m)[3] = 0;
	(mv.m)[7] = 0;
	(mv.m)[11] = 0;
	mvp = Mult(projectionMatrix, mv);
	glUniformMatrix4fv(glGetUniformLocation(skyProgram, "mvp"), 1, GL_TRUE, mvp.m);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, skytex);

	glBindTexture(GL_TEXTURE_2D, skytex1);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( -0.5f, -0.5f, -0.5f );
        glTexCoord2f(1, 0); glVertex3f( -0.5f, -0.5f,  0.5f );
        glTexCoord2f(1, 1); glVertex3f( -0.5f,  0.5f,  0.5f );
        glTexCoord2f(0, 1); glVertex3f( -0.5f,  0.5f, -0.5f );
    glEnd();

    glBindTexture(GL_TEXTURE_2D, skytex2);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(  0.5f, -0.5f,  0.5f );
        glTexCoord2f(1, 0); glVertex3f(  0.5f, -0.5f, -0.5f );
        glTexCoord2f(1, 1); glVertex3f(  0.5f,  0.5f, -0.5f );
        glTexCoord2f(0, 1); glVertex3f(  0.5f,  0.5f,  0.5f );
    glEnd();

    glBindTexture(GL_TEXTURE_2D, skytex3);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex3f( -0.5f,  0.5f, -0.5f );
        glTexCoord2f(0, 0); glVertex3f( -0.5f,  0.5f,  0.5f );
        glTexCoord2f(1, 0); glVertex3f(  0.5f,  0.5f,  0.5f );
        glTexCoord2f(1, 1); glVertex3f(  0.5f,  0.5f, -0.5f );
    glEnd();

    glBindTexture(GL_TEXTURE_2D, skytex4);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( -0.5f, -0.5f, -0.5f );
        glTexCoord2f(0, 1); glVertex3f( -0.5f, -0.5f,  0.5f );
        glTexCoord2f(1, 1); glVertex3f(  0.5f, -0.5f,  0.5f );
        glTexCoord2f(1, 0); glVertex3f(  0.5f, -0.5f, -0.5f );
    glEnd();

    glBindTexture(GL_TEXTURE_2D, skytex5);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(  0.5f, -0.5f, -0.5f );
        glTexCoord2f(1, 0); glVertex3f( -0.5f, -0.5f, -0.5f );
        glTexCoord2f(1, 1); glVertex3f( -0.5f,  0.5f, -0.5f );
        glTexCoord2f(0, 1); glVertex3f(  0.5f,  0.5f, -0.5f );
    glEnd();

    glBindTexture(GL_TEXTURE_2D, skytex6);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( -0.5f, -0.5f,  0.5f );
        glTexCoord2f(1, 0); glVertex3f(  0.5f, -0.5f,  0.5f );
        glTexCoord2f(1, 1); glVertex3f(  0.5f,  0.5f,  0.5f );
        glTexCoord2f(0, 1); glVertex3f( -0.5f,  0.5f,  0.5f );
 
    glEnd();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//DrawModel(skyboxm, skyProgram, "inPosition", NULL, "inTexCoord");
	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	

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