	#include "CameraControl.h"

	GLfloat StandardSpeed = 0;
	GLfloat RunSpeed = 0;
	GLint WindowWidth = 0;
	GLint WindowHeight = 0;

	void CameraControlsInit(GLfloat standardSpeed, GLfloat runSpeed, GLint windowWidth, GLint windowHeight)
	{
		StandardSpeed = standardSpeed;
		RunSpeed = runSpeed;
		WindowWidth = windowWidth;
		WindowHeight = windowHeight;
	}



	vec3 GetCurrentCameraPosition(mat4 camBaseMatrix)
	{
		return SetVector(-(camBaseMatrix.m)[3], -(camBaseMatrix.m)[7], -(camBaseMatrix.m)[11]);
	}

	vec3 GetBackDirectionVec(mat4 camMatrix)
	{
		mat4 directions = Transpose(camMatrix); //Taking the inverse, kinda pointless
		return Normalize(SetVector( (directions.m)[2], (directions.m)[6], (directions.m)[10]));
	}

	vec3 GetRightDirectionVec(mat4 camMatrix)
	{
		mat4 directions = Transpose(camMatrix); //Taking the inverse, kinda pointless
		return Normalize(SetVector( (directions.m)[0], (directions.m)[4], (directions.m)[8]));	
	}

	vec3 GetUpDirectionVec(mat4 camMatrix)
	{
		mat4 directions = Transpose(camMatrix); //Taking the inverse, kinda pointless
		return Normalize(SetVector( (directions.m)[1], (directions.m)[5], (directions.m)[9]));
	}

	vec3 GetNewUpDirectionVec(vec3 currentPosition)
	{
		vec3 upDirection = VectorSub(currentPosition, middleOfPlanet);
		return Normalize(upDirection);
	}

	mat4 CameraMouseUpdate(GLint mouseX, GLint mouseY, mat4 camMatrix, mat4 camBaseMatrix)
	{	
			static GLfloat x = 0;
			static GLfloat y = 0;	

			//Update mousepointer value
			if (mouseY != 0)
			{
				x = (GLfloat)mouseX;

				//Clamp Y
				if (mouseY < 375 && mouseY > 276)
				y = (GLfloat)mouseY;
			}

			camMatrix = Mult(Ry(2*M_PI*x/512), Mult( ArbRotate(GetRightDirectionVec(camMatrix), 2*M_PI*y/324), camBaseMatrix));		
			return camMatrix;
	}


	//Read keyboard input and update camera position
	mat4 CameraControl(GLint t, mat4 camMatrix, mat4 camBaseMatrix)
	{
		static GLfloat averageSpeed;
		if (averageSpeed <= 0)
		{
			averageSpeed = StandardSpeed;
		}
		static GLint tlast = 0;
		GLint passedTime = t - tlast;
		tlast = t;	
		
		GLfloat speed = (GLfloat)passedTime * averageSpeed;
		

		vec3 backDirectionVec = GetBackDirectionVec(camMatrix);
		vec3 rightDirectionVec = GetRightDirectionVec(camMatrix);

		//Cant move in y direction, y direction removed, speed added
		vec3 backVec = ScalarMult( Normalize(SetVector(backDirectionVec.x, 0, backDirectionVec.z)), speed);
		vec3 rightVec = ScalarMult( Normalize(SetVector(rightDirectionVec.x, 0, rightDirectionVec.z)), speed);
		if (glutKeyIsDown('w') || glutKeyIsDown('W'))
			camBaseMatrix = Mult( T( backVec.x, backVec.y, backVec.z), camBaseMatrix);
		if (glutKeyIsDown('a') || glutKeyIsDown('A')) 
			camBaseMatrix = Mult( T( rightVec.x, rightVec.y, rightVec.z), camBaseMatrix);
		if (glutKeyIsDown('s') || glutKeyIsDown('S'))
			camBaseMatrix = Mult( T( -backVec.x, -backVec.y, -backVec.z), camBaseMatrix);
		if (glutKeyIsDown('d') || glutKeyIsDown('D'))
			camBaseMatrix = Mult( T( -rightVec.x, -rightVec.y, -rightVec.z), camBaseMatrix);
		if (glutKeyIsDown('q') || glutKeyIsDown('Q'))
			camBaseMatrix = Mult( T( 0, speed, 0), camBaseMatrix);
		if (glutKeyIsDown('e') || glutKeyIsDown('E'))
			camBaseMatrix = Mult( T( 0, -speed, 0), camBaseMatrix);
		
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
					if (averageSpeed == StandardSpeed)
					{
						averageSpeed = RunSpeed;
					}
					else
					{
						averageSpeed = StandardSpeed;
					}
					zPressed = true;
				}
			}
			else
			{
				zPressed = false;
			}
		}

		
		return camBaseMatrix;
	}



