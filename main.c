#include "MicroGlut.h"
#include "Init.h"
#include "UpdateAndDisplay.h"

#include "DisplayGlobals.h"

#include <signal.h>

void Init(void)
{
	// GL Inits
	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL Inits");

	InitAll();

	glUseProgram(modelProgram);
	glUniformMatrix4fv(glGetUniformLocation(modelProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

	InitModels();
}

int main(int argc, char **argv)
{
	atexit(cleanUpAndExit);
	signal(SIGTERM, exit);
	signal(SIGINT, exit);



	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize (windowWidth, windowHeight);
	glutCreateWindow ("Planet");
	glutHideCursor();
	glutWarpPointer(windowWidth*0.5, windowHeight*0.5);
	glutDisplayFunc(display);
	glutReshapeFunc(InitWindow);
	Init();

	globalTime = glutGet(GLUT_ELAPSED_TIME);
	timer(globalTime);
	
	glutMainLoop();
	exit(0);
}


