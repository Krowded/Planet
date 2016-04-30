#include "CameraControl.h"
#include "Physics.h"
#include "ControlGlobals.h"
#include "DisplayGlobals.h"
#include "PlanetManager.h"
#include "PlayAudioFile.h"

// Forward declaration of local functions
LOCAL vec3 GetBackDirectionVec(mat4 camRotatedMatrix);
LOCAL vec3 GetRightDirectionVec(mat4 camRotatedMatrix);
LOCAL vec3 GetUpDirectionVec(mat4 camRotatedMatrix);
LOCAL vec3 GetOldUpDirectionVec(mat4 camRotatedMatrix);
LOCAL vec3 GetNewUpDirectionVec(mat4 camRotatedMatrix, struct PlanetStruct planet);
LOCAL mat4 ChangeUpDirection(mat4 camPositionMatrix, mat4 camRotatedMatrix, vec3 newUpDirection, GLint t);

LOCAL mat4 CameraControl(GLint t, mat4 camRotatedMatrix, mat4 camPositionMatrix, struct PlanetStruct planet);
LOCAL mat4 AdjustCameraToHeightMap(mat4 camPositionMatrix, struct PlanetStruct planet);

LOCAL mat4 mouseRotationMatrix = {{
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1	}};



char* deletePlanetNoise = "audio/Blop-Mark_DiAngelo-79054334.mp3";

vec3 GetCurrentCameraPosition(mat4 camPositionMatrix)
{
	mat4 inverseMat = InvertMat4(camPositionMatrix); 
	return SetVector((inverseMat.m)[3], (inverseMat.m)[7], (inverseMat.m)[11]);
}

/*
 * 	Move camera to a specific position
 */
mat4 SetCameraPosition(mat4 camPositionMatrix, vec3 position)
{
	mat4 inverseMat = InvertMat4(camPositionMatrix); 
	(inverseMat.m)[3] = position.x;
	(inverseMat.m)[7] = position.y;
	(inverseMat.m)[11] = position.z;
	return InvertMat4(inverseMat);
}


/*
 *	Gets the backwards direction relative to where the camera is looking
 */
LOCAL vec3 GetBackDirectionVec(mat4 camRotatedMatrix)
{
	//mat4 directions = InvertMat4(camRotatedMatrix); //Only needed if there is scaling?
	mat4 directions = Transpose(camRotatedMatrix);
	return Normalize(SetVector( (directions.m)[2], (directions.m)[6], (directions.m)[10]));
}

/*
 *	Gets the right direction relative to where the camera is looking
 */
LOCAL vec3 GetRightDirectionVec(mat4 camRotatedMatrix)
{
	//mat4 directions = InvertMat4(camRotatedMatrix);
	mat4 directions = Transpose(camRotatedMatrix);
	return Normalize(SetVector( (directions.m)[0], (directions.m)[4], (directions.m)[8]));	
}


/*
 *	Gets the up direction relative to where the camera is looking
 */
LOCAL vec3 GetUpDirectionVec(mat4 camRotatedMatrix)
{
	//mat4 directions = InvertMat4(camRotatedMatrix);
	mat4 directions = Transpose(camRotatedMatrix);
	return Normalize(SetVector( (directions.m)[1], (directions.m)[5], (directions.m)[9]));
}


/*
 *	
 */
LOCAL vec3 GetOldUpDirectionVec(mat4 camRotatedMatrix)
{
	static vec3 upVec = {0, 1, 0};
	if( IsGravityOn() )
	{
		upVec = GetUpDirectionVec(camRotatedMatrix);
	}
	return upVec;
}


/*
 *	Gets the desired up vector
 *	No change if gravity off, normal vector from center of planet to camera otherwise	
 */
LOCAL vec3 GetNewUpDirectionVec(mat4 camPositionMatrix, struct PlanetStruct planet)
{
	static vec3 upvec = {0, 1, 0};
	if(IsGravityOn())
	{
		upvec = Normalize(VectorSub(GetCurrentCameraPosition(camPositionMatrix), planet.center));	
	}
	return upvec;
}


/*
 *	Smoothly changes the Up direction of the camera to newUpVector
 */
LOCAL mat4 ChangeUpDirection(mat4 camPositionMatrix, mat4 camRotatedMatrix, vec3 newUpVector, GLint t)
{
	static GLfloat tlast = 0;
	GLfloat passedTime = t - tlast;
	tlast = t;

	static vec3 axis = {0, 1, 0};
	static GLfloat oldAngle = 0;

	//TODO: Make transition smooth when switching between planets
	/*
	if( NewGravity == GL_TRUE ) 
	{
		vec3 currentUpVector = GetUpDirectionVec(camRotatedMatrix);
		oldAngle = acos( DotProduct(currentUpVector, newUpVector) );
		NewGravity = GL_FALSE;
	}*/
	
	vec3 oldUpVector = GetOldUpDirectionVec(camPositionMatrix);
	
	GLfloat newAngle = acos( DotProduct(oldUpVector, newUpVector));

	if( newAngle > 0.0001 && newAngle < M_PI - 0.0001)
	{
		axis = Normalize( CrossProduct(oldUpVector, newUpVector) );

		//Stop from instant spinning
		if (fabs(newAngle -  oldAngle) > (maxRotationSpeed*passedTime))
		{
			if( newAngle - oldAngle > 0 )
			{
				newAngle = oldAngle + (maxRotationSpeed*passedTime);
			}	
			else
			{
				newAngle = oldAngle - (maxRotationSpeed*passedTime);
			}
		}
		camPositionMatrix = Mult( ArbRotate(axis, -newAngle), camPositionMatrix);

		oldAngle = newAngle;
	}
	return camPositionMatrix;
}


/*
 *	Rotates the camera with the mouse movement, locks mouse pointer in middle of screen
 */
void CameraMouseUpdate(GLint mouseX, GLint mouseY)
{		
	static GLfloat x = 0;
	static GLfloat y = 0;
	
	if ( IsTopWindow() )	
	{
		x += mouseX - windowWidth*0.5;
		
		GLint check = (fabs(y + mouseY - windowHeight*0.5)*4.0*mouseSensitivity < 1);
		if (check) 
		{
			y += mouseY - windowHeight*0.5;
		}

		mouseRotationMatrix = Mult( Rx(2.0*M_PI*y*mouseSensitivity), Ry(2.0*M_PI*x*mouseSensitivity));		

		glutWarpPointer(windowWidth*0.5, windowHeight*0.5);
	}
}


/* 
 * Updates camBaseMatrix and camMatrix as input and physics dictates
 */
void UpdateCamera(GLint t, struct PlanetStruct planet)
{
	//Read input and update position
	camBaseMatrix = CameraControl(t, camMatrix, camBaseMatrix, planet);
	camBaseMatrix = AdjustCameraToHeightMap(camBaseMatrix, 
											planet);//GetTerrainHeight(currentPosition, terrainModel, planet.terrainTexture[0]));
	
	//Update orientation
	vec3 newUp = GetNewUpDirectionVec(camBaseMatrix, planet);
	mat4 camRotatedMatrix = ChangeUpDirection(camBaseMatrix, camMatrix, newUp, t);
	camRotatedMatrix = Mult(mouseRotationMatrix, camRotatedMatrix);


	camMatrix = camRotatedMatrix;
}


/*
 *	Reads keyboard input
 * 	Responsible for player movement
 */
LOCAL mat4 CameraControl(GLint t, mat4 camRotatedMatrix, mat4 camPositionMatrix, struct PlanetStruct planet)
{
	static GLfloat averageSpeed;
	//Deal with player movement
	{ //scope limiter
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
	}


	//Switch current planet	
	{ //static scope limiter
		static bool gravitySwitchPressed = false;
		bool noKeysPressed = true;
		GLint i;
		for(i = 0; i < 9; i++)
		{
			if(glutKeyIsDown((GLchar)(i + '0')))
			{
				if(!gravitySwitchPressed) SetCurrentPlanet(i);
				gravitySwitchPressed = true;
				noKeysPressed = false;
			}
		}
		
		if (noKeysPressed)
		{
			gravitySwitchPressed = false;
		}
	}	

	//Create a new planet 300 units in front of camera
	{ //static scope limiter
		static bool planetKeyPressed = false;
		if(glutKeyIsDown('r') || glutKeyIsDown('R')  || glutMouseIsDown(GLUT_KEY_RIGHT))
		{
			if(!planetKeyPressed)
			{
				struct PlanetStruct planet;
				planet.center = VectorAdd(GetCurrentCameraPosition(camPositionMatrix), ScalarMult(GetBackDirectionVec(camRotatedMatrix), -300));
				planet.radius = (256/2)/(GetNumberOfPlanets()+1);
				planet.orbitalSpeed = 0.001;
				planet.orbitalAxis = SetVector(0,1,0);
				planet.rotationalSpeed = 0.001;
				planet.rotationalAxis = SetVector(1,1,0);
				planet.type = ROUGH_PLANET;

				CreatePlanet(planet, PLAY_SOUND_1);
				planetKeyPressed = true;
			}
		}
		else
		{
			planetKeyPressed = false;
		}
	}

	//Remove last created planet
	{ //static scope limiter
		static bool removePlanetKeyPressed = false;
		if(glutKeyIsDown('t') || glutKeyIsDown('T') || glutMouseIsDown(GLUT_KEY_RIGHT))
		{
			if(!removePlanetKeyPressed)
			{
				RemoveLastPlanet();
				PlayAudioFile(deletePlanetNoise);  //Should probably play inside RemoveLastPlanet() or something rather than here
				removePlanetKeyPressed = true;
			}
		}
		else
		{
			removePlanetKeyPressed = false;
		}
	}


	//Turn gravity on and off
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

	//Turn high speed on and off
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

/*
 *	Simulates gravity and normal force by moving the camera to slightly above ground
 */
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