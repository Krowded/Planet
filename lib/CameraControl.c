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
		//return SetVector(-(camBaseMatrix.m)[3], -(camBaseMatrix.m)[7], -(camBaseMatrix.m)[11]);


		mat4 inverseMat = InvertMat4(camBaseMatrix);
		vec3 pos = SetVector((inverseMat.m)[3], (inverseMat.m)[7], (inverseMat.m)[11]);
		return pos;

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

	mat4 SetUpDirectionVec(mat4 camMatrix, vec3 newUpVector)
	{
		mat4 tempVec = Transpose(camMatrix);
		(tempVec.m)[1] = newUpVector.x;
		(tempVec.m)[5] = newUpVector.y;
		(tempVec.m)[9] = newUpVector.z;

		int i;
		fprintf(stderr, "camMatrixOrig: ");
		for(i = 0; i < 12; i++)
		{
			fprintf(stderr, " %f", (camMatrix.m)[i]);
		}
		fprintf(stderr, "\n");
		camMatrix = Transpose(tempVec);

		fprintf(stderr, "camMatrixNew: ");
		for(i = 0; i < 12; i++)
		{
			fprintf(stderr, " %f", (camMatrix.m)[i]);
		}
		fprintf(stderr, "\n");

		return camMatrix;
	}

	vec3 GetNewUpDirectionVec(mat4 camMatrix)
	{
		vec3 upvec = Normalize(VectorSub(GetCurrentCameraPosition(camMatrix), middleOfPlanet));	
		vec3 currentP = GetCurrentCameraPosition(camMatrix);
		//fprintf(stderr, "Current: X: %f, Y: %f, Z: %f\n", currentP.x, currentP.y, currentP.z);
		//fprintf(stderr, "UP X: %f, Y: %f, Z: %f\n", upvec.x, upvec.y, upvec.z);
		return upvec;
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

			vec3 newUp = GetNewUpDirectionVec(camMatrix);
			camMatrix = SetUpDirectionVec(camBaseMatrix, newUp);
			//fprintf(stderr, "newup: x %f y %f z %f\n", newUp.x, newUp.y, newUp.z);
	

			/*GLfloat angle = acos(DotProduct(GetUpDirectionVec(camMatrix), GetCurrentCameraPosition(camMatrix))/(Norm(GetUpDirectionVec(camMatrix)) * Norm(GetCurrentCameraPosition(camMatrix))));
			vec3 axis = CrossProduct(GetUpDirectionVec(camMatrix), GetCurrentCameraPosition(camMatrix));
			camMatrix = Mult( ArbRotate(axis, angle), camMatrix);
*/
			camMatrix = Mult(ArbRotate(newUp, (2*M_PI*x/512)), Mult( ArbRotate(GetRightDirectionVec(camMatrix), 2*M_PI*y/324), camMatrix));		
			return camMatrix;
	}


	mat4 RotateArbAxis(mat4 matrix, vec3 axis, GLfloat angle)
	{
		axis = Normalize(axis);
		mat4 Rot;
		(Rot.m)[0] = cos(angle) + pow(axis.x, 2) * (1.0 - cos(angle));
		(Rot.m)[1] = axis.x * axis.y * (1 - cos(angle)) - axis.z * sin(angle);
		(Rot.m)[2] = axis.x * axis.z * (1 - cos(angle)) + axis.y * sin(angle);
		(Rot.m)[3] = 0.0;

		(Rot.m)[4] = axis.x * axis.y * (1 - cos(angle)) + axis.z * sin(angle);
		(Rot.m)[5] = cos(angle) + pow(axis.z, 2) * (1 - cos(angle));
		(Rot.m)[6] = axis.y * axis.z * (1 - cos(angle)) - axis.x * sin(angle);
		(Rot.m)[7] = 0.0;

		(Rot.m)[8] = axis.x * axis.z * (1 - cos(angle)) - axis.y * sin(angle);
		(Rot.m)[9] = axis.y * axis.z * (1 - cos(angle)) + axis.x * sin(angle);
		(Rot.m)[10] = axis.x * axis.z * (1 - cos(angle)) + axis.y * sin(angle);
		(Rot.m)[11] = 1.0;

		mat4 tempMatrix = matrix;
		(tempMatrix.m)[3] = 0.0;
		(tempMatrix.m)[7] = 0.0;
		(tempMatrix.m)[11] = 1.0;

		tempMatrix = Mult(Rot, tempMatrix);

		(tempMatrix.m)[3] = (matrix.m)[3];
		(tempMatrix.m)[7] = (matrix.m)[7];
		(tempMatrix.m)[11] = (matrix.m)[11];

		return tempMatrix;
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



