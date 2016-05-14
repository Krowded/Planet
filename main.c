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
}

int main(int argc, char **argv)
{
	//Set up proper exit and signal handling
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

	timer(glutGet(GLUT_ELAPSED_TIME));
	
	glutMainLoop();
	exit(0);
}


