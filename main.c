#include "MicroGlut.h"
#include "Init.h"
#include "UpdateAndDisplay.h"


void init(void)
{
	// GL inits
	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	initAll();

	glUseProgram(modelProgram);
	glUniformMatrix4fv(glGetUniformLocation(modelProgram, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

	initModels();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize (windowWidth, windowHeight);
	glutCreateWindow ("Project");
	//glutHideCursor();
	glutWarpPointer(windowWidth*0.5, 0);
	glutDisplayFunc(display);
	init();

	globalTime = glutGet(GLUT_ELAPSED_TIME);
	timer(globalTime);

	glutPassiveMotionFunc(CamUpdate);

	glutMainLoop();
	exit(0);
}
