	#include "CameraControl.h"

	vec3 GetCurrentCameraPosition(mat4 camPositionMatrix)
	{
		mat4 inverseMat = InvertMat4(camPositionMatrix);
		vec3 pos = SetVector((inverseMat.m)[3], (inverseMat.m)[7], (inverseMat.m)[11]);
		return pos;

	}

	vec3 GetBackDirectionVec(mat4 camRotatedMatrix)
	{
		mat4 directions = InvertMat4(camRotatedMatrix); //Taking the inverse, kinda pointless
		return Normalize(SetVector( (directions.m)[2], (directions.m)[6], (directions.m)[10]));
	}

	vec3 GetRightDirectionVec(mat4 camRotatedMatrix)
	{
		mat4 directions = InvertMat4(camRotatedMatrix); //Taking the inverse, kinda pointless
		return Normalize(SetVector( (directions.m)[0], (directions.m)[4], (directions.m)[8]));	
	}

	vec3 GetUpDirectionVec(mat4 camRotatedMatrix)
	{
		if( IsGravityOn() )
		{
			mat4 directions = InvertMat4(camRotatedMatrix); //Taking the inverse, kinda pointless
			return Normalize(SetVector( (directions.m)[1], (directions.m)[5], (directions.m)[9]));
		}
		else
		{
			return SetVector(0, 1, 0);
		}
	}

	mat4 ChangeUpDirection(mat4 camPositionMatrix, vec3 newUpVector)
	{
		vec3 oldUpVector = GetUpDirectionVec(camPositionMatrix);

		GLfloat angle = acos( DotProduct(oldUpVector, newUpVector));
		if( angle > 0.0001)
		{
			vec3 axis = Normalize( CrossProduct(oldUpVector, newUpVector) );
			camPositionMatrix = Mult( ArbRotate(axis, -angle), camPositionMatrix);
		}

		return camPositionMatrix;
	}

	vec3 GetNewUpDirectionVec(mat4 camPositionMatrix)
	{
		if(IsGravityOn())
		{
			vec3 upvec = Normalize(VectorSub(GetCurrentCameraPosition(camPositionMatrix), middleOfPlanet));	
			return upvec;
		}
		else
		{
			return SetVector(0,1,0);
		}

	}

	mat4 CameraMouseUpdate(GLint mouseX, GLint mouseY, mat4 camRotatedMatrix, mat4 camPositionMatrix)
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
			
			vec3 newUp = GetNewUpDirectionVec(camPositionMatrix);
			camRotatedMatrix = ChangeUpDirection(camPositionMatrix, newUp);

			vec3 upvec = SetVector(0,1,0);//GetNewUpDirectionVec(camPositionMatrix);
			vec3 rightvec = SetVector(1,0,0);//GetRightDirectionVec(camRotatedMatrix);
			camRotatedMatrix = Mult( ArbRotate(upvec, (2*M_PI*x/512)), camRotatedMatrix);
			
			fprintf(stderr, "rightvec: x %f y %f z %f\n", rightvec.x, rightvec.y, rightvec.z);
			camRotatedMatrix = Mult( ArbRotate(rightvec, 2*M_PI*y/324), camRotatedMatrix);		

			return camRotatedMatrix;
	}

	//Read keyboard input and update camera position
	mat4 CameraControl(GLint t, mat4 camRotatedMatrix, mat4 camPositionMatrix)
	{
		static GLfloat averageSpeed;
		if (averageSpeed <= 0)
		{
			averageSpeed = standardSpeed;
		}
		static GLint tlast = 0;
		GLint passedTime = t - tlast;
		tlast = t;	
		
		GLfloat speed = (GLfloat)passedTime * averageSpeed;
		

		vec3 backDirectionVec = GetBackDirectionVec(camRotatedMatrix);
		vec3 rightDirectionVec = GetRightDirectionVec(camRotatedMatrix);

		//Can only move perpendicular to planet -> remove projection onto normal
		vec3 upVec = GetNewUpDirectionVec(camPositionMatrix);

		vec3 backVec = Normalize(SetVector(backDirectionVec.x, backDirectionVec.y, backDirectionVec.z));
		vec3 backVecProj = ScalarMult(upVec, DotProduct(backVec, upVec));
		backVec = VectorSub( backVec, backVecProj);
		backVec = ScalarMult(backVec, speed);

		vec3 rightVec = Normalize(SetVector(rightDirectionVec.x, rightDirectionVec.y, rightDirectionVec.z));
		vec3 rightVecProj = ScalarMult(upVec, DotProduct(rightVec, upVec));
		rightVec = VectorSub( rightVec, rightVecProj);
		rightVec = ScalarMult(rightVec, speed);
		
		if (glutKeyIsDown('w') || glutKeyIsDown('W'))
			camPositionMatrix = Mult( T( backVec.x, backVec.y, backVec.z), camPositionMatrix);
		if (glutKeyIsDown('a') || glutKeyIsDown('A')) 
			camPositionMatrix = Mult( T( rightVec.x, rightVec.y, rightVec.z), camPositionMatrix);
		if (glutKeyIsDown('s') || glutKeyIsDown('S'))
			camPositionMatrix = Mult( T( -backVec.x, -backVec.y, -backVec.z), camPositionMatrix);
		if (glutKeyIsDown('d') || glutKeyIsDown('D'))
			camPositionMatrix = Mult( T( -rightVec.x, -rightVec.y, -rightVec.z), camPositionMatrix);
		if (glutKeyIsDown('q') || glutKeyIsDown('Q'))
			camPositionMatrix = Mult( T( 0, speed, 0), camPositionMatrix);
		if (glutKeyIsDown('e') || glutKeyIsDown('E'))
			camPositionMatrix = Mult( T( 0, -speed, 0), camPositionMatrix);


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


	mat4 AdjustCameraToHeightMap(mat4 camPositionMatrix, GLfloat height)
	{
		if( IsGravityOn() )
		{
			height = cameraHeight + height;
			camPositionMatrix = AdjustModelToHeightMap(camPositionMatrix, GetCurrentCameraPosition(camPositionMatrix), height);
		}

		return camPositionMatrix;
	}


