#include "CameraControl.h"

LOCAL mat4 mouseRotationMatrix = {
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1	};

vec3 GetCurrentCameraPosition(mat4 camPositionMatrix)
{
	mat4 inverseMat = InvertMat4(camPositionMatrix); 
	return SetVector((inverseMat.m)[3], (inverseMat.m)[7], (inverseMat.m)[11]);
}

LOCAL vec3 GetBackDirectionVec(mat4 camRotatedMatrix)
{
	//mat4 directions = InvertMat4(camRotatedMatrix); //Only needed if there is scaling?
	mat4 directions = Transpose(camRotatedMatrix);
	return Normalize(SetVector( (directions.m)[2], (directions.m)[6], (directions.m)[10]));
}

LOCAL vec3 GetRightDirectionVec(mat4 camRotatedMatrix)
{
	//mat4 directions = InvertMat4(camRotatedMatrix);
	mat4 directions = Transpose(camRotatedMatrix);
	return Normalize(SetVector( (directions.m)[0], (directions.m)[4], (directions.m)[8]));	
}

LOCAL vec3 GetUpDirectionVec(mat4 camRotatedMatrix)
{
	//mat4 directions = InvertMat4(camRotatedMatrix);
	mat4 directions = Transpose(camRotatedMatrix);
	return Normalize(SetVector( (directions.m)[1], (directions.m)[5], (directions.m)[9]));
}

LOCAL vec3 GetOldUpDirectionVec(mat4 camRotatedMatrix)
{
	static vec3 upVec = {0, 1, 0};
	if( IsGravityOn() )
	{
		upVec = GetUpDirectionVec(camRotatedMatrix);
	}
	return upVec;
}

LOCAL vec3 GetNewUpDirectionVec(mat4 camPositionMatrix, struct PlanetStruct planet)
{
	static vec3 upvec = {0, 1, 0};
	if(IsGravityOn())
	{
		upvec = Normalize(VectorSub(GetCurrentCameraPosition(camPositionMatrix), planet.center));	
	}
	return upvec;
}

//Changes the updirection of the camera
LOCAL mat4 ChangeUpDirection(mat4 camPositionMatrix, mat4 camRotatedMatrix, vec3 newUpVector)
{
	static vec3 axis = {0, 1, 0};
	static GLfloat oldAngle = 0;
	vec3 oldUpVector = GetOldUpDirectionVec(camPositionMatrix);
	
	GLfloat newAngle = acos( DotProduct(oldUpVector, newUpVector));
		
	if( newAngle > 0.0001 && newAngle < M_PI - 0.0001)
	{
		axis = Normalize( CrossProduct(oldUpVector, newUpVector) );

		vec3 currentUpVector = GetUpDirectionVec(camRotatedMatrix);
		GLfloat relativeAngle =  acos( DotProduct(currentUpVector, newUpVector) );

		//Stop from instant spinning
		if (abs(newAngle -  oldAngle) > maxRotationSpeed)
			if( newAngle - oldAngle > 0 )
			{
				newAngle = oldAngle + maxRotationSpeed;
			}
			else
			{
				newAngle = oldAngle - maxRotationSpeed;
			}
		
		camPositionMatrix = Mult( ArbRotate(axis, -newAngle), camPositionMatrix);

		oldAngle = newAngle;
	}
	return camPositionMatrix;
}


void CameraMouseUpdate(GLint mouseX, GLint mouseY)
{		
	static GLfloat x = 0;
	static GLfloat y = 0;
	
	if ( IsTopWindow() )	
	{
		x += mouseX - windowWidth*0.5;
		
		GLint check = (abs(y + mouseY - windowHeight*0.5)*4.0*mouseSensitivity < 1);
		if (check) 
		{
			y += mouseY - windowHeight*0.5;
		}

		mouseRotationMatrix = Mult( Rx(2.0*M_PI*y*mouseSensitivity), Ry(2.0*M_PI*x*mouseSensitivity));		

		glutWarpPointer(windowWidth*0.5, windowHeight*0.5);
	}
}

void UpdateCamera(GLint t, struct PlanetStruct planet)
{
	//Read input and update position
	camBaseMatrix = CameraControl(t, camMatrix, camBaseMatrix, planet);
	camBaseMatrix = AdjustCameraToHeightMap(camBaseMatrix, 
											planet);//GetTerrainHeight(currentPosition, terrainModel, planet.terrainTexture[0]));
	
	//Update orientation
	vec3 newUp = GetNewUpDirectionVec(camBaseMatrix, planet);
	mat4 camRotatedMatrix = ChangeUpDirection(camBaseMatrix, camMatrix, newUp);
	camRotatedMatrix = Mult(mouseRotationMatrix, camRotatedMatrix);


	camMatrix = camRotatedMatrix;
}


//Read keyboard input
LOCAL mat4 CameraControl(GLint t, mat4 camRotatedMatrix, mat4 camPositionMatrix, struct PlanetStruct planet)
{
	static GLfloat averageSpeed;
	static GLint tlast = 0;

	if (averageSpeed <= 0)
	{
		averageSpeed = standardSpeed;
	}
	
	GLint passedTime = t - tlast;
	tlast = t;	
	
	GLfloat speed = (GLfloat)passedTime * averageSpeed;

	vec3 upVec = GetUpDirectionVec(camRotatedMatrix);
	vec3 backVec = GetBackDirectionVec(camRotatedMatrix);
	vec3 rightVec = GetRightDirectionVec(camRotatedMatrix);

	vec3 pos = GetCurrentCameraPosition(camPositionMatrix);
	//fprintf(stderr, "Position: x %f y %f z %f\n", pos.x, pos.y, pos.z);

	//Can only move perpendicular to planet -> remove projection onto normal
	if ( IsGravityOn() )
	{
		vec3 gravUpVec = GetNewUpDirectionVec(camPositionMatrix, planet);

		vec3 backVecProj = ScalarMult(gravUpVec, DotProduct(backVec, gravUpVec));
		backVec = Normalize(VectorSub( backVec, backVecProj));
		
		vec3 rightVecProj = ScalarMult(gravUpVec, DotProduct(rightVec, gravUpVec));
		rightVec = Normalize(VectorSub( rightVec, rightVecProj));
	}
	backVec = ScalarMult(backVec, speed);		
	rightVec = ScalarMult(rightVec, speed);
	upVec = ScalarMult(upVec, speed);
	
	if (glutKeyIsDown('w') || glutKeyIsDown('W'))
		camPositionMatrix = Mult( T( backVec.x, backVec.y, backVec.z), camPositionMatrix);
	if (glutKeyIsDown('a') || glutKeyIsDown('A')) 
		camPositionMatrix = Mult( T( rightVec.x, rightVec.y, rightVec.z), camPositionMatrix);
	if (glutKeyIsDown('s') || glutKeyIsDown('S'))
		camPositionMatrix = Mult( T( -backVec.x, -backVec.y, -backVec.z), camPositionMatrix);
	if (glutKeyIsDown('d') || glutKeyIsDown('D'))
		camPositionMatrix = Mult( T( -rightVec.x, -rightVec.y, -rightVec.z), camPositionMatrix);
	if (glutKeyIsDown('q') || glutKeyIsDown('Q'))
		camPositionMatrix = Mult( T( upVec.x, upVec.y, upVec.z), camPositionMatrix);
	if (glutKeyIsDown('e') || glutKeyIsDown('E'))
		camPositionMatrix = Mult( T( -upVec.x, -upVec.y, -upVec.z), camPositionMatrix);
	if (glutKeyIsDown(GLUT_KEY_ESC))
		cleanUpAndExit();

	//Switch current planet
	if(glutKeyIsDown('0'))
	{
		SetCurrentPlanet(0);
	}
	else if(glutKeyIsDown('1'))
	{
		SetCurrentPlanet(1);
	}
	else if (glutKeyIsDown('2'))
	{
		SetCurrentPlanet(2);
	}
	else if (glutKeyIsDown('3'))
	{
		SetCurrentPlanet(3);
	}	
	else if (glutKeyIsDown('4'))
	{
		SetCurrentPlanet(4);
	}	
	else if (glutKeyIsDown('5'))
	{
		SetCurrentPlanet(5);
	}	
	else if (glutKeyIsDown('6'))
	{
		SetCurrentPlanet(6);
	}	
	else if (glutKeyIsDown('7'))
	{
		SetCurrentPlanet(7);
	}	
	else if (glutKeyIsDown('8'))
	{
		SetCurrentPlanet(8);
	}	

	{ //static scope limiter
		static bool planetKeyPressed = false;
		if(glutKeyIsDown('r') || glutKeyIsDown('R')  || glutMouseIsDown(GLUT_KEY_RIGHT))
		{
			if(!planetKeyPressed)
			{
			/* Create in a formation
				vec3 center;
				if(numberOfPlanets < 6)
					center = SetVector(cos((numberOfPlanets+1)*2*M_PI/5)*500, 0, sin((numberOfPlanets+1)*2*M_PI/5)*500);
				else if(numberOfPlanets < 11) 
					center = SetVector(cos((numberOfPlanets-4)*2*M_PI/5)*500, sin((numberOfPlanets-4)*2*M_PI/5)*500, 0);
				else
					center = SetVector(0, cos((numberOfPlanets-9)*2*M_PI/5)*500, sin((numberOfPlanets-9)*2*M_PI/5)*500); 
				*/

				//Create planet where you're looking
				vec3 center = VectorAdd(GetCurrentCameraPosition(camPositionMatrix), ScalarMult(GetBackDirectionVec(camRotatedMatrix), -300));

				//CreatePlanet(center, planetsList[0].radius/numberOfPlanets, planetsList[0].upVec, planetsList[0].frontVec); //Buggy if number of planets == 0
				CreatePlanet(center, (256/2)/(numberOfPlanets+1), SetVector(0,1,0), SetVector(0,0,1));
				fprintf(stderr, "Let there be light!\n");
				planetKeyPressed = true;
			}
		}
		else
		{
			planetKeyPressed = false;
		}
	}

	{ //static scope limiter
		static bool removePlanetKeyPressed = false;
		if(glutKeyIsDown('t') || glutKeyIsDown('T') || glutMouseIsDown(GLUT_KEY_RIGHT))
		{
			if(!removePlanetKeyPressed)
			{
				RemoveLastPlanet();
				removePlanetKeyPressed = true;
			}
		}
		else
		{
			removePlanetKeyPressed = false;
		}
	}



	{ //static scope limiter
		static bool spacePressed = false;
		if (glutKeyIsDown(' '))
		{
			if (!spacePressed)
			{
				SetGravity(!IsGravityOn());
				spacePressed = true;
			}
		}
		else
		{
			spacePressed = false;
		}
	}

	{	//static scope limiter
		static bool zPressed = false;
		if (glutKeyIsDown('z') || glutKeyIsDown('Z'))
		{
			if(!zPressed)
			{
				if (averageSpeed == standardSpeed)
				{
					averageSpeed = runSpeed;
				}
				else
				{
					averageSpeed = standardSpeed;
				}
				zPressed = true;
			}
		}
		else
		{
			zPressed = false;
		}
	}

	return camPositionMatrix;
}


LOCAL mat4 AdjustCameraToHeightMap(mat4 camPositionMatrix, struct PlanetStruct planet)
{
	if( IsGravityOn() )
	{
		vec3 currentPosition = GetCurrentCameraPosition(camPositionMatrix);
		vec3 upvec = Normalize(VectorSub(currentPosition, planet.center));
		currentPosition = VectorSub(currentPosition, ScalarMult(upvec, cameraHeight)); //Pretend the player is further down, to push him up to cameraHeight
		camPositionMatrix = AdjustModelToHeightMap(camPositionMatrix, currentPosition, planet);
	}

	return camPositionMatrix;
}


/* If we want gravity from several planets:

Loop through planetsList, apply directional movement depending on distance. 
Then compare distances and check for collision with the nearest one only*/